
CREATE Table ShipManufacturer(
	nkey integer PRIMARY KEY,
	sname varchar(35) NOT NULL
);

GRANT ALL ON ShipManufacturer TO void;

INSERT INTO ShipManufacturer (nkey,sname) VALUES (0,'IM&V');
INSERT INTO ShipManufacturer (nkey,sname) VALUES (1,'N-Tech');
INSERT INTO ShipManufacturer (nkey,sname) VALUES (2,'DPT');
INSERT INTO ShipManufacturer (nkey,sname) VALUES (3,'USST');
INSERT INTO ShipManufacturer (nkey,sname) VALUES (4,'Void');


CREATE TABLE ShipType(
	nkey integer PRIMARY KEY,
	kmanufacturer integer REFERENCES ShipManufacturer(nkey) NOT NULL,
	sname varchar(30) NOT NULL,
	nmaxmissiles integer,
	nmaxshields integer,
	nmaxattack integer,
	nmaxholds integer,
	ninitholds integer,
	nmaxsentinels integer,
	nmaxtrackers integer,
	nmaxmines integer,
	nmaxpeople integer,
	nmaxprobes integer,
	nturnspersector integer,
	bwarpdrive boolean,
	bcloakable boolean,
	banalyzer boolean,
	nscandistance integer,
	ncost integer,
	nforecolor integer,
	nbackcolor integer,
	ntransrange integer,
	bforsale boolean,
	CHECK (nforecolor >=0 and nforecolor <=15),
	CHECK (nbackcolor >=0 and nbackcolor <=7),
	CHECK (ninitholds <= nmaxholds)
);

GRANT ALL ON ShipType TO void;

INSERT INTO ShipType VALUES(0, 0, 'Explorer'  ,50,200,25,30,15,20,3,3, 20,  0, 1,false,false,false,0,15000,0,7,3, TRUE);
INSERT INTO ShipType VALUES(1, 0, 'Freedom'   ,80,200,35,45,25,30,5,5, 80,  0, 2,false,false,false,0,25000,1,7,4, TRUE);
INSERT INTO ShipType VALUES(2, 0, 'Galaxy'  ,150,400,50,60,30,50,7,10, 150, 5, 3,false,true ,false,0,50000,4,7, 5,TRUE);
INSERT INTO ShipType VALUES(3, 0, 'Universe',275,900,75,75,50,75,10,10,250, 10,3,false,true ,true, 0,90000,5,7,5,TRUE);

INSERT INTO ShipType VALUES(4, 1, 'Sistica',  100,100,35, 45,25,30, 10,10,50,  1, 1,false,false,false,0,30000,7,4,4,TRUE);
INSERT INTO ShipType VALUES(5, 1, 'Andromeda',300,300,100,60,40,60, 15,25,100, 7, 2,false,true,false, 0,100000,12,4,6,TRUE);
INSERT INTO ShipType VALUES(6, 1, 'Belarus', 1000,1250,250,80,50,100,20,20,150,10, 2,true,true,true, 1, 350000,13,4,7,TRUE);

INSERT INTO ShipType VALUES(7, 2, 'Betelguese',    600 ,350, 200, 75, 50,150, 15,15,100,8, 3, false,true,false,0,115000,13,1,4,TRUE);
INSERT INTO ShipType VALUES(8, 2, 'Betelguese II', 1800,1300,500, 100,70,500, 25,25,150,10,3, false,true,true,1,375000, 14,1,5,TRUE);
INSERT INTO ShipType VALUES(9, 2, 'Betelguese III',5000,3000,1500,125,80,1200,25,25,150,10,4, false,true,true,1,700000, 15,1,6, TRUE);

INSERT INTO ShipType VALUES(10,3, 'Escape Pod',15,100,5,10,5,0,0,0,0,0,1,false,false,false,0,30000,1,0,1,FALSE);
INSERT INTO ShipType VALUES(11,3, 'SC-1',    350,400,100,75,50,125,20,20,100, 8, 3, false, true,false, 0, 100000, 3,0,4,TRUE);
INSERT INTO ShipType VALUES(12,3, 'SD-1',    800,800,200,125,75,200,15,30,175, 8, 3, false, true,false, 0, 200000, 6,0,4,TRUE);
INSERT INTO ShipType VALUES(13,3, 'CV-1',    600,700,150,150,75,175,15,30,250, 8, 3, false, true,false, 0, 200000, 2,0,3,TRUE);
INSERT INTO ShipType VALUES(14,3, 'Solar',    2000,2000,500,150,75,500,20,50,200,10, 3, false, true,true, 1, 500000, 10,0,5,TRUE);
INSERT INTO ShipType VALUES(15,3, 'Solar500', 5000,5000,1200,150,75, 1000,20,75,225,10,4,true,true,true,1,1000000,11,0,6,TRUE);
INSERT INTO ShipType VALUES(16,3, 'SuperNova',12000,10000,2500,175,75,2000,20,125,250,12,5,true,true,true,1,2500000,12,0,7,TRUE);
INSERT INTO ShipType VALUES(17,3, 'SuperNova 2',15000,15000,3000,175,75,2200,22,150,250,12,5,true,true,true,1,3000000,13,0,8,TRUE);
INSERT INTO ShipType VALUES(18,3, 'SuperNova DE',18000,15000,3000,175,75,2500,22,200,275,12,6,true,true,true,3,5000000,14,0,8,TRUE);

INSERT INTO ShipType VALUES(19,4, 'Alpha',50000,50000,7000,200,100,6000,30,500,20,500,10,true,true,true,4,20000000,14,6,10,FALSE );
INSERT INTO ShipType VALUES(20,4, 'Beta',150000,200000,10000,300,150,9000,30,1000,700,30,15,true,true,true,6,50000000,15,6,10,FALSE);
