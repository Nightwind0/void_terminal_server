CREATE TABLE Player(
sname varchar(30) PRIMARY KEY,
klogin varchar(20) REFERENCES Login(SLOGIN) NOT NULL,
bmob boolean,
ncredits integer,
nturnsleft integer,
npoints integer,
kalliance varchar(30) REFERENCES Alliance(SNAME),
kcurrentship integer REFERENCES Ship(nkey),
dfirstplay timestamp,
dlastplay timestamp
);
GRANT ALL ON Player to void;


-- Add in our circular references here. 
ALTER TABLE Alliance ADD FOREIGN KEY ( kleader) REFERENCES Player(sname);
ALTER TABLE Ship ADD FOREIGN KEY( kowner) REFERENCES Player(sname);
