CREATE TABLE Mob(
	sname varchar(30) PRIMARY KEY,
	npersonality integer,
	nhomesector integer REFERENCES Sectors(nsector),
	ndest integer REFERENCES Sectors(nsector),
	
	CHECK (npersonality >=0 and npersonality <= 6)
);

GRANT ALL ON Mob to void;
