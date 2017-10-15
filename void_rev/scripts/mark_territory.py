#/usr/bin/python
import psycopg2
import optparse
import sys

distance = 3
database = "void_large"

parser = optparse.OptionParser()
parser.set_defaults(instance='default',command='list')
parser.add_option('--db', action='store_true', dest='database')
parser.add_option('--distance', action='store_true', dest='distance')
(options, args) = parser.parse_args()

if options.distance:
    distance = options.distance
if options.database:
    database = options.database

try:
        
        conn=psycopg2.connect("dbname='%s' user='void' password='tiTVPok?'" % (database))
except:
        print "I am unable to connect to the database."
        sys.exit(1)



def mark_adjacent_territory(sector, territory, cur_depth, max_depth):
        cur = conn.cursor()
        cur.execute("UPDATE Sectors set kterritory = %s where nsector = %s;", (territory, sector))
        cur.execute("select nsector2 from edges where nsector = %s union select nsector from edges where nsector2 = %s;", (sector,sector))
        for row in cur:
                adj = row[0]
                if cur_depth < max_depth:
                        mark_adjacent_territory(adj, territory, cur_depth +1, max_depth)
                
def mark_territory():
        cur = conn.cursor()
        # Find all stardocks, plus sector 0, mark all adjacent as federation. 
        cur.execute("SELECT nsector FROM Sectors WHERE bstardock = true;")
        for row in cur:
                sector = row[0]
                mark_adjacent_territory(sector, 0, 0, distance)
        # terra is fed territory
        mark_adjacent_territory(0, 0, 0, distance)


mark_territory()

