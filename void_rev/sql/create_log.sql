CREATE TABLE Log (
	dstamp  timestamp NOT NULL,
	nseverity  integer NOT NULL,
	smessage  text NOT NULL,
	CHECK (nseverity >=0 AND nseverity <6)
);

GRANT ALL ON Log TO void;
