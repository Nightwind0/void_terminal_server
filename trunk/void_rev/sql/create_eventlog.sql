create table EventLog(
	dstamp timestamp not null,
	sactor varchar(30) references Player(sname),
	ssubject varchar(30),
	ntype Integer  not null,
	smessage varchar(80),
	kshiptye Integer references ShipType(nkey),
	sshipname varchar(30)
	
);
CREATE INDEX eventlog_dstamp ON EventLog(dstamp);
GRANT ALL ON EventLog to void;
