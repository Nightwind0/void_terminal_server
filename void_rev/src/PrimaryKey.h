#ifndef VOID_PRIMARY_KEY_H
#define VOID_PRIMARY_KEY_H

#include <vector>
#include "Field.h"






class PrimaryKey
{
 protected:
    std::vector<Field*> m_fields;
    

 public:
    PrimaryKey();
    PrimaryKey(Field *field);
    PrimaryKey(Field *field1,Field *field2);
    PrimaryKey(Field *field1, Field *field2, Field *field3);
    ~PrimaryKey();
    
    void SetFields(const std::vector<Field*> &fields);
    std::vector<Field*>::const_iterator GetFieldsBegin() const;
    std::vector<Field*>::const_iterator GetFieldsEnd() const;
    int FieldCount()const;
   
    
    PrimaryKey(const PrimaryKey &key); // copy constructor

    bool operator==(PrimaryKey &other) const;
    const PrimaryKey & operator=(const PrimaryKey &other);
    

    

};



#endif
