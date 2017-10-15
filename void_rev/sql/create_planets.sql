CREATE TABLE IF NOT EXISTS PlanetClass (
       sname VARCHAR(30) PRIMARY KEY,
       fplasmamultiplier real NOT NULL,
       fmetalsmultiplier real NOT NULL,
       fcarbonmultiplier real NOT NULL,
       fmeansize real NOT NULL DEFAULT 1.0,
       fvariance real NOT NULL DEFAULT 0.4,
       bhabitable boolean NOT NULL DEFAULT true,
       nmaxcivlevel integer NOT NULL,
       ndefaultflags integer DEFAULT 0,
       fprevalence real NOT NULL DEFAULT 0.01,
       fdailygrowth integer NOT NULL DEFAULT 0
);


GRANT ALL ON PlanetClass to void;
GRANT ALL ON PlanetClass to postgres;

INSERT INTO PlanetClass (sname, fplasmamultiplier, fmetalsmultiplier, fcarbonmultiplier, fmeansize, fvariance, bhabitable, nmaxcivlevel, ndefaultflags, fprevalence, fdailygrowth) VALUES
       ('Terrestrial', 0, 0, 0, 1.0, 0.4, true, 5, 1, 0.005, 500);
       
INSERT INTO PlanetClass (sname, fplasmamultiplier, fmetalsmultiplier, fcarbonmultiplier, fmeansize, fvariance, bhabitable, nmaxcivlevel, ndefaultflags, fprevalence) VALUES
       ('Volcanic', 1.3, 1.2, 0.1, 0.8, 0.4, true, 3, 0, 0.0075);

INSERT INTO PlanetClass (sname, fplasmamultiplier, fmetalsmultiplier, fcarbonmultiplier, fmeansize, fvariance, bhabitable, nmaxcivlevel, ndefaultflags, fprevalence) VALUES
       ('Desert', 1.2, 1.2, 0.3, 0.7, 0.4, true, 5, 0, 0.01);

INSERT INTO PlanetClass (sname, fplasmamultiplier, fmetalsmultiplier, fcarbonmultiplier, fmeansize, fvariance, bhabitable, nmaxcivlevel, ndefaultflags, fprevalence) VALUES
       ('Dwarf', 1.0, 1.4, 0.6, 0.5, 0.2, true, 5, 0, 0.001);

INSERT INTO PlanetClass (sname, fplasmamultiplier, fmetalsmultiplier, fcarbonmultiplier, fmeansize, fvariance, bhabitable, nmaxcivlevel, ndefaultflags, fprevalence, fdailygrowth) VALUES
       ('Crystalline', 0.5, 0.5, 0.5, 0.5, 0.2, true, 4, 1, 0.0002, 300);

INSERT INTO PlanetClass (sname, fplasmamultiplier, fmetalsmultiplier, fcarbonmultiplier, fmeansize, fvariance, bhabitable, nmaxcivlevel, ndefaultflags, fprevalence, fdailygrowth) VALUES
       ('Dyson Sphere', 1.0, 1.0, 1.0, 3.0, 0.8, true, 5, 1, 0.0001, 500);

INSERT INTO PlanetClass (sname, fplasmamultiplier, fmetalsmultiplier, fcarbonmultiplier, fmeansize, fvariance, bhabitable, nmaxcivlevel, ndefaultflags, fprevalence) VALUES
       ('Swamp', 0.1, 0.1, 1.4, 0.8, 0.4, true, 5, 0, 0.01);

INSERT INTO PlanetClass (sname, fplasmamultiplier, fmetalsmultiplier, fcarbonmultiplier, fmeansize, fvariance, bhabitable, nmaxcivlevel, ndefaultflags, fprevalence) VALUES
       ('Forest', 1.0, 1.0, 1.5, 1.2, 0.4, true, 4, 0, 0.001);

INSERT INTO PlanetClass (sname, fplasmamultiplier, fmetalsmultiplier, fcarbonmultiplier, fmeansize, fvariance, bhabitable, nmaxcivlevel, ndefaultflags, fprevalence) VALUES
       ('Gas Giant', 0, 0, 0, 20.0, 6.0, false, 0, 0, 0.015);




CREATE TABLE IF NOT EXISTS Planet (
       ksector integer REFERENCES Sectors(nsector) NOT NULL,
       sname varchar(30) NOT NULL,
       fsize real NOT NULL,
       kclass varchar(30) REFERENCES PlanetClass(sname) NOT NULL,
       fpopulation real DEFAULT 0.0,
       fplasma integer default 0.0,
       fmetals integer default 0.0,
       fcarbon integer default 0.0,
       ncivlevel integer DEFAULT 0,
       dcivstart timestamp,
       dstartconstruction timestamp,
       nflags integer DEFAULT 0,
       kowner varchar(30) REFERENCES Player(sname),
       kalliance varchar(30) REFERENCES Alliance(sname),
       klastvisitor varchar(30) REFERENCES Player(sname),
       kcreator varchar(30) REFERENCES Player(sname),
       dlastvisit timestamp,
       dcreation timestamp,
       dlastupdate timestamp,
       PRIMARY KEY(ksector,sname)
);

GRANT ALL ON Planet TO void;
GRANT ALL ON Planet to postgres;
