CREATE TABLE Territory (
NKEY Integer PRIMARY KEY,
SNAME varchar(128) UNIQUE NOT NULL
);

GRANT ALL ON Territory to void;
