CREATE Table Login (
slogin varchar(20) PRIMARY KEY,
spassword varchar(256) NOT NULL,
dfirstlogin timestamp,
dlastlogin timestamp,
slastip varchar(20),
semail varchar(80),
bdisabled boolean,
nlogins integer
);
GRANT ALL On Login To Void;
