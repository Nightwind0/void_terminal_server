

create table Mail(
	nmailid integer PRIMARY KEY,
	dstamp timestamp not null,
	ktoplayer varchar(30) references Player(sname) not null,
	kfromplayer varchar(30) references Player(sname),
	bfromsystem boolean,
	smessage varchar(1024)
);
CREATE SEQUENCE mail_id_seq CYCLE;
CREATE INDEX mail_dstamp_idx ON Mail(dstamp);
CREATE INDEX mail_to_idx ON Mail(ktoplayer);
CREATE INDEX mail_id_idx ON Mail(nmailid);
GRANT ALL ON Mail to void;
GRANT ALL ON mail_id_seq to void;
