
-- Get All Adjacent Sectors for Sector 29
select nsector2 from edges where nsector = 29 union select nsector from edges where nsector2 = 29;



-- Show a list of all sectors and how many edges they each have
select sector, count(0) as links from (select nsector as sector from edges union all select nsector2 from edges as sector) as f group by sector;


-- One row per link      
select nsector from edges union all select nsector2 from edges order by
nsector;


-- Count one-deeps
select count(0) from (select sector, count(0) as links from (select nsector as sector from edges union all select nsector2 from edges as sector) 
as f group by sector) as sub1 where links =1;

-- Show one-deeps
select sector from (select sector, count(0) as links from (select nsector as sector from edges union all select nsector2 from edges as sector) 
as f group by sector) as sub1 where links =1;

-- Show links for sector 895
select nsector2 as sector from edges where nsector = 895 union select nsector as sector from edges where nsector2 = 895;



-- Show two-deeps (actually shows the one-deep connected to a two-deep)
select nsector from edges where (nsector in (select sector from (select sector, count(0) as links from (select nsector as sector from edges union all select nsector2 from edges as sector) as f group by sector) as sub1 where links =1)  and nsector2 in (select sector from (select sector, count(0) as links from (select nsector as sector from edges union all select nsector2 from edges as sector) as f2 group by sector) as sub2 where links =2)) or (nsector2 in (select sector from (select sector, count(0) as links from (select nsector as sector from edges union all select nsector2 from edges as sector) as f3 group by sector) as sub1 where links =1)  and nsector in (select sector from (select sector, count(0) as links from (select nsector as sector from edges union all select nsector2 from edges as sector) as f3 group by sector) as sub3 where links =2)) ;


