begin;
update outpost set fplasmaprice = fplasmaprice / (select cast(svalue as real) from config where sname = 'base_plasma_price'),
fmetalsprice = fmetalsprice / (select cast(svalue as real) from config where sname = 'base_metals_price'), fcarbonprice = fcarbonprice / (select cast(svalue as real) from config where sname = 'base_carbon_price');
end;
