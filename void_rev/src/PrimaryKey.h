#ifndef VOID_PRIMARY_KEY_H
#define VOID_PRIMARY_KEY_H

#include <vector>
#include "Field.h"






class PrimaryKey
{
 protected:
    std::vector<FieldPtr> m_fields;
    

 public:
    PrimaryKey();
    PrimaryKey(FieldPtr field);
    PrimaryKey(const std::string& dbname, int field);
    PrimaryKey(const std::string& dbname, const std::string& field);
    PrimaryKey(const std::string& dbname, double field);
    PrimaryKey(FieldPtr field1, FieldPtr field2);
    PrimaryKey(FieldPtr field1, FieldPtr field2, FieldPtr field3);
    ~PrimaryKey();
    
    void SetFields(const std::vector<FieldPtr> &fields);
    std::vector<FieldPtr>::const_iterator GetFieldsBegin() const;
    std::vector<FieldPtr>::const_iterator GetFieldsEnd() const;
    int FieldCount()const;
   
    
    PrimaryKey(const PrimaryKey &key); // copy constructor

    bool operator==(PrimaryKey &other) const;
    const PrimaryKey & operator=(const PrimaryKey &other);
    

    

};



#endif
