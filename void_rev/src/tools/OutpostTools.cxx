#include "OutpostTools.h"
#include "ResourceMaster.h"

double OutpostTools::GetBuyRateAfterTime(unsigned int minutes, double current_price)
{
    ResourceMaster * RM = ResourceMaster::GetInstance();

    int delay = std::stoi(RM->GetConfig("price_change_delay").c_str());

    if( minutes < delay ) return current_price;
    else minutes -= delay;

    const double rate_cap = atof(RM->GetConfig("buyrate_cap").c_str());
    const double gap_ratio = atof(RM->GetConfig("gap_ratio").c_str());
    const double stock_recovery_minutes = atof(RM->GetConfig("stock_recovery_minutes").c_str());
 
    double result =   rate_cap - ((rate_cap - current_price) * pow(gap_ratio, (double)minutes / stock_recovery_minutes)) ;

    RM->Log(AUDIT,"#BUY P=" + DoubleToString(current_price) + "  I=" + IntToString(minutes) + " P'=" + DoubleToString(result)) ;

    return result;
}


double OutpostTools::GetSellRateAfterTime(unsigned int minutes, double current_price)
{
    ResourceMaster * RM = ResourceMaster::GetInstance();

    int delay = std::stoi(RM->GetConfig("price_change_delay").c_str());

    if( minutes < delay ) return current_price;
    else minutes -= delay;

    const double gap_ratio = atof(RM->GetConfig("gap_ratio").c_str());
    const double stock_recovery_minutes = atof(RM->GetConfig("stock_recovery_minutes").c_str());
    const double rate_floor = atof(RM->GetConfig("sellrate_floor").c_str());
    

    double result = (current_price - rate_floor) * pow(gap_ratio, (double)minutes / stock_recovery_minutes) + rate_floor;

    RM->Log(AUDIT,"#SELL P=" + DoubleToString(current_price) + "  I=" + IntToString(minutes) + " P'=" + DoubleToString(result)) ;

    return result;
}

double OutpostTools::GetBuyRateAfterPurchase(unsigned int stock, double current_price)
{
    ResourceMaster * RM = ResourceMaster::GetInstance();
    double r = std::stof(RM->GetConfig("buy_price_delta_per_unit").c_str());
    double i = std::stof(RM->GetConfig("stock_unit").c_str());
    double f = std::stof(RM->GetConfig("sellrate_floor").c_str());
    
    double result = (current_price - f) * pow(r,(double)stock/i) + f;

    RM->Log(AUDIT,"#PCH P=" + DoubleToString(current_price) + "  S=" + IntToString(stock) + " P'=" + DoubleToString(result)) ;
    

    return result;
}

double OutpostTools::GetSellRateAfterSale(unsigned int stock, double current_price)
{
    ResourceMaster * RM = ResourceMaster::GetInstance();
    double r = config_float(RM,"sell_price_delta_per_unit");
    double i = config_float(RM,"stock_unit");
    double f = config_float(RM,"sellrate_floor");
    double a = config_float(RM,"sellrate_linear_delta");

//    p' = p + a*s/i

    double linearprice = current_price + a * stock / i;
    double exp_price = (current_price - f) * pow(r,(double)stock/i) + f;

    double result = std::max(linearprice,exp_price);
    
    RM->Log(AUDIT,"#SLE P=" + DoubleToString(current_price) + "  S=" + DoubleToString(stock) + " P'=" + DoubleToString(result)) ;

    return result;
}
