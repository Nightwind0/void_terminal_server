CREATE TABLE Alliance(
	sname varchar(30) PRIMARY KEY,
	kleader varchar(30),
	bmobrun boolean,
	ncredits integer,
	nhomesector integer REFERENCES Sectors(nsector),
	kterritory integer REFERENCES Territory(nkey) 
);

GRANT ALL ON Alliance to void;

--ALTER TABLE Alliance ADD CONSTRAINT kleader REFERENCES Player(sname)
