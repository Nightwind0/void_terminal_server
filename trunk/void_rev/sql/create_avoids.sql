create table avoids(
	kplayer varchar(30) references Player(sname),
	ksector integer references Sectors(nsector),
	primary key(kplayer,ksector)
);
create index avoids_kplayer_idx on avoids(kplayer);
create index avoids_ksector_idx on avoids(ksector);

grant all on avoids to void;
