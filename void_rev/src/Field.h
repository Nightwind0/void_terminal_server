#ifndef VOID_FIELD_H
#define VOID_FIELD_H


#include <string>
#include <pqxx/pqxx>
#include <memory>

enum FIELD_TYPE{INTEGER,TEXT,FLOAT, TIMESTAMP, BOOLEAN};
class Field;
using FieldPtr = std::shared_ptr<Field>;

class Field
{
 protected:
    std::string m_data;
    std::string m_name;
    bool m_isnull;

 public:
    Field(const std::string &dbname);
    Field(const std::string &dbname, const std::string &value, bool isnull=false){ m_name = dbname; m_isnull = isnull; SetFromString(value);}
    virtual ~Field(){}

    void SetFromString(const std::string &str){m_data = str;};
    std::string GetAsString()const{ return m_data;}
    bool IsNull()const { return m_isnull;};
    void SetNull(){ m_isnull = true;}
    virtual FieldPtr clone()const=0;


    std::string name()const { return m_name; }
    virtual FIELD_TYPE GetType()const =0;

    
    bool operator<(const Field &other) const
	    {
		    return GetAsString() < other.GetAsString();
	    }
    
    bool operator==(const Field &other) const
	    {
		    if(other.GetType() == GetType() &&  GetAsString() == other.GetAsString())
			    return true;
		    else return false;
	    }

    bool operator!=(const Field & other) const
	    {
		    return !operator==(other);
	    }
    
    
};


class Timestamp : public Field
{
 private:

    

 public:
//    Timestamp(const Timestamp &ts){ *this = ts }
    Timestamp(const std::string dbname, const std::string &value):Field(dbname,value){}
    Timestamp(const std::string &dbname, const std::string &value, bool isnull):Field(dbname,value,isnull){}
    ~Timestamp(){}
//    virtual std::string GetAsString() const;

    FIELD_TYPE GetType()const{ return TIMESTAMP;}
    int GetMonth()const;
    int GetYear()const;
    int GetDay()const;
    int GetMinute()const;
    int GetHour()const;
    int GetSecond()const;
    virtual FieldPtr clone()const{ return std::make_shared<Timestamp>(m_name,m_data,m_isnull);}
    operator std::string(){ return GetAsString(); }


};

class Text: public Field
{
 private:
  

 public:
    //  Text(const Text &ts){ *this = ts }
    Text(const std::string dbname, const std::string &value):Field(dbname,value){}
    Text(const std::string &dbname, const std::string &value, bool isnull):Field(dbname,value,isnull){}
    ~Text(){}
    FIELD_TYPE GetType()const{ return TEXT;}
    virtual FieldPtr clone()const{ return std::make_shared<Text>(m_name,m_data,m_isnull);}
    //  virtual std::string GetAsString() const;
    operator std::string() const { return GetAsString(); } 

};

class Integer: public Field
{
 private:
	

 public:
//	Integer(const Integer &ts){ *this = ts }
	Integer(const std::string dbname, const std::string &value):Field(dbname,value){}
	Integer(const std::string &dbname, const std::string &value, bool isnull):Field(dbname,value,isnull){}

	FIELD_TYPE GetType()const{ return INTEGER;}
	virtual FieldPtr  clone()const{ return std::make_shared<Integer>(m_name,m_data,m_isnull);}
//	virtual std::string GetAsString() const;

	
	int GetValue(){ return atoi(m_data.c_str());}
	operator int(){ return GetValue();}
};


class Boolean: public Field
{
 private:


 public:
//	Boolean(const Boolean &ts){ *this = ts }
	Boolean(const std::string dbname, const std::string &value):Field(dbname,value){}
	Boolean(const std::string &dbname, const std::string &value, bool isnull):Field(dbname,value,isnull){}
	FIELD_TYPE GetType()const{ return BOOLEAN;}
//	virtual std::string GetAsString() const;
	virtual FieldPtr clone()const{ return std::make_shared<Boolean>(m_name,m_data,m_isnull);}

	bool GetValue(){return (m_data == "t");}
	operator bool(){ return GetValue(); }
};

class Float: public Field
{
 private:


 public:
//	Float(const Float &ts){ *this = ts }
	Float(const std::string dbname, const std::string &value):Field(dbname,value){}
	Float(const std::string &dbname, const std::string &value, bool isnull):Field(dbname,value,isnull){}
	FIELD_TYPE GetType()const{ return FLOAT;}
//	virtual std::string GetAsString() const;
	virtual FieldPtr clone()const{ return std::make_shared<Float>(m_name,m_data,m_isnull);}

	double GetValue(){return atof(m_data.c_str());}
	operator double(){ return GetValue();}
};







#endif
