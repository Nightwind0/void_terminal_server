CREATE TABLE Sectors(
NSECTOR Integer PRIMARY KEY,
KTERRITORY Integer REFERENCES Territory(NKEY),
BDISCOVERED Boolean,
BSTARDOCK Boolean,
SSTARDOCKNAME Varchar(60)
);
