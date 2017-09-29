-- Economy knobs
insert into Config (sname, svalue) values ('price_change_delay', 360);
insert into Config (sname, svalue) values ('buyrate_cap', 1.5);
insert into Config (sname, svalue) values ('gap_ratio', 0.5);
insert into Config (sname, svalue) values ('stock_recovery_minutes', 60 * 5);
insert into Config (sname, svalue) values ('sellrate_floor', 1.0);
insert into Config (sname, svalue) values ('stock_unit', 5);
insert into Config (sname, svalue) values ('sell_price_delta_per_unit', 0.1);
insert into Config (sname, svalue) values ('sellrate_linear_delta', 1.0);

