CREATE TABLE Outpost (
sname varchar(60) PRIMARY KEY,
ksector integer REFERENCES Sectors(nsector) NOT NULL,
bspecial BOOLEAN,
bbuyplasma BOOLEAN NOT NULL,
bbuymetals BOOLEAN NOT NULL,
bbuycarbon BOOLEAN NOT NULL,
kdiscoverer varchar(30) REFERENCES Player(sname),
klastvisitor varchar(30) REFERENCES Player(sname),
dlastvisit timestamp,
ndaystocompletion integer
);
GRANT ALL ON Outpost to void;


ALTER TABLE Outpost ADD COLUMN nplasmaprice integer;
ALTER TABLE Outpost ADD COLUMN nmetalsprice integer;
ALTER TABLE Outpost ADD COLUMN ncarbonprice integer;
