create table sentinels
(
	ksector integer references Sectors(nsector),
	kplayer varchar(30) references Player(sname),
	ncount integer not null,
	primary key(ksector,kplayer)
);

create index sent_sec_idx on sentinels(ksector);
create index sent_plyr_idx on sentinels(kplayer);
grant all on sentinels to void;
