CREATE TABLE SectorFlags (
kplayer varchar(30),
ksector integer REFERENCES Sectors(nsector) NOT NULL,
nflags integer,
PRIMARY KEY(kplayer,ksector)
);

CREATE INDEX sector_flags_ksector ON SectorFlags USING btree(ksector);

GRANT ALL ON SectorFlags to void;
