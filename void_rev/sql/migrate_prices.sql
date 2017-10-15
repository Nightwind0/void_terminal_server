-- WARNING: Don't run this unless you for some reason have an old version
-- database that you need to migrate.

-- This migrates the OLD style of prices, where the prices were direct
-- values in the database, to the new style where we instead store
-- multipliers for price, and the base price in the config

begin;
update outpost set fplasmaprice = fplasmaprice / (select cast(svalue as real) from config where sname = 'base_plasma_price'),
fmetalsprice = fmetalsprice / (select cast(svalue as real) from config where sname = 'base_metals_price'), fcarbonprice = fcarbonprice / (select cast(svalue as real) from config where sname = 'base_carbon_price');
end;
