CREATE TABLE Ship(
nkey integer PRIMARY KEY,
sname varchar(30) NOT NULL,
ktype integer REFERENCES ShipType(nkey) NOT NULL,
ballianceowned boolean,
kowner varchar(30),
kalliance varchar(30) REFERENCES Alliance(sname),
bpersonal boolean,
nsentinels integer,
nmissiles integer,
nmines integer,
ntrackers integer,
nshields integer,
nplasma integer,
nmetals integer,
ncarbon integer,
npeople integer,
ndebris integer,
nexplosives integer,
nprobes integer,
nholds integer,
bcloaked boolean,
ktowship integer REFERENCES Ship(nkey),
ksector integer REFERENCES Sectors(nsector)
);



CREATE INDEX ship_ktow_idx on Ship (ktowship);

GRANT ALL ON Ship to void;



-- After player creation, you have to add the constraint to this table
-- ALTER TABLE Ship ADD FOREIGN KEY (kowner) REFERENCES Player(sname);
--kmanufacturer varchar(30) REFERENCES ShipManufacturer(sname) NOT NULL,
