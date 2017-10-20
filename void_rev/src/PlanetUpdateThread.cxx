
#include <chrono>
#include <thread>
#include "PlanetHandle.h"
#include "PlanetUpdateThread.h"
#include "ResourceMaster.h"
#include <unistd.h>
#include <iostream>

const std::string kQueryPlanetStmt { "getPlanetInfoTimeStmt" };
const std::string kPlanetUpdateStmt { "planetUpdateStmt" };

PlanetUpdateThread::PlanetUpdateThread(){
}

PlanetUpdateThread::~PlanetUpdateThread(){
}

bool PlanetUpdateThread::thread_init(){
  std::string sockname = "/tmp/void-planet-update-" + ResourceMaster::GetInstance()->GetInstanceName();
  unlink(sockname.c_str());

  m_usocket = std::make_shared<UNIXDatagramSocket>(sockname);
  
  m_usocket->Create();
  m_usocket->Bind(0,sockname);

  return true;
}

void PlanetUpdateThread::thread_destroy(){
  m_usocket->Close();
  std::cerr << "Planet update thread shutting down..." << std::endl;
}


double PlanetUpdateThread::CalculatePopulation(double current, double daily_growth, double seconds)const{
  return current + (daily_growth / (60*60*24)) * double(seconds);
}

double PlanetUpdateThread::CalculateResource(double current, double multiplier, double population, double seconds, double divisor)const{
  return current + ((population * double(seconds)) / divisor) * multiplier;
}

bool PlanetUpdateThread::run(){
  try {
    std::unique_ptr<pqxx::connection_base> conn = ResourceMaster::GetInstance()->CreateDatabaseConnection();
    conn->prepare(kQueryPlanetStmt, "select planet.sname, ksector, extract(epoch from now() - dlastupdate), fpopulation, fplasmamultiplier, fmetalsmultiplier, fcarbonmultiplier, fdailygrowth, fplasma, fmetals, fcarbon from planet, planetclass  where planet.kclass = planetclass.sname;");
    conn->prepare(kPlanetUpdateStmt, "update planet set fpopulation = $1, fplasma = $2, fmetals = $3, fcarbon = $4, dlastupdate = now() where sname = $5 and ksector = $6;");
  
    using UpdateInfo = std::tuple<double,double,double,double,std::string,int>;
    
    while(true){
      std::vector<UpdateInfo> updates;
      double resource_divisor = std::stoi(ResourceMaster::GetInstance()->GetConfig("resource_generation_divisor"));
      pqxx::read_transaction readtxn(*conn);
      pqxx::result query_result = readtxn.prepared(kQueryPlanetStmt).exec();
      for(const auto& row : query_result){
	const double elapsed_secs = row[2].as<double>(0.0);
	const double pop = row[3].as<double>(0.0);
	const double plasma_mult = row[4].as<double>(1.0);
	const double metals_mult = row[5].as<double>(1.0);
	const double carbon_mult = row[6].as<double>(1.0);
	const double daily_growth = row[7].as<double>(1.0);
	const double plasma = row[8].as<double>(0.0);
	const double metals = row[9].as<double>(0.0);
	const double carbon = row[10].as<double>(0.0);
	const int new_pop = CalculatePopulation(pop, daily_growth, elapsed_secs);
	const double new_plasma = CalculateResource(plasma, plasma_mult, new_pop, elapsed_secs, resource_divisor);
	const double new_metals = CalculateResource(metals, metals_mult, new_pop, elapsed_secs, resource_divisor);
	const double new_carbon = CalculateResource(carbon, carbon_mult, new_pop, elapsed_secs, resource_divisor);
	updates.emplace_back(new_pop, new_plasma, new_metals, new_carbon, row[0].as<std::string>(), row[1].as<int>());
      }
      readtxn.commit();

      for(const auto& update_info : updates){
	pqxx::work work{*conn};
	double new_pop, new_plasma, new_metals, new_carbon;
	std::string name;
	int sector;
	std::tie(new_pop, new_plasma, new_metals, new_carbon, name, sector) = update_info;
	work.prepared(kPlanetUpdateStmt)(new_pop)(new_plasma)(new_metals)(new_carbon)(name)(sector).exec();
	work.commit();
      }
      Socket::eSelectResult result = m_usocket->Select(60);
      if(result == Socket::eSelectResult::THIS_SOCKET){
	char buffer[1024];
	int bytes = m_usocket->RecvFrom(buffer,sizeof(buffer) / sizeof(char), 0);
	return true;
      }
    }

  }catch(DBException &dbe){
    std::cerr << dbe.GetMessage() << std::endl;
    ResourceMaster::GetInstance()->Log(EMERGENCY, dbe.GetMessage());
    return false;
  }catch(std::exception& e){
    std::cerr << e.what() << std::endl;
    ResourceMaster::GetInstance()->Log(EMERGENCY, e.what());
    return false;
  }catch(...){
    return false;
  }
  
}
