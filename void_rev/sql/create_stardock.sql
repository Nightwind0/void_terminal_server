CREATE TABLE Stardock (
SNAME varchar(60) PRIMARY KEY,
ksector integer REFERENCES Sectors(nsector) NOT NULL
);

GRANT ALL ON Stardock to void;
