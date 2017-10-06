#!/usr/bin/python

import os
import optparse
import psycopg2
import random
import sys
import math
from quadtree import Quadtree
from quadtree import Circle
from Queue import PriorityQueue
from collections import defaultdict

numsectors = 10000
numstardocks = 16
outpost_prevalence = 0.10
plasma_price = 45
metals_price = 30
carbon_price = 20
buy_range = (0.8, 1.2)
sell_range = (0.6, 1.0)
database = "void_large"
connectivity = 6
emptiness = 20
connect_range =  emptiness * connectivity

parser = optparse.OptionParser()
parser.set_defaults(instance='default',command='list')
parser.add_option('--instance', action='store_true', dest='instance')
parser.add_option('--db', action='store_true', dest='database')
parser.add_option('--sectors', action='store_true', dest='sectors')
parser.add_option('--stardocks', action='store_true', dest='stardocks')
parser.add_option('--connectivity', action='store_true', dest='connectivity')
parser.add_option('--connect_range', action='store_true', dest='connect_range')
(options, args) = parser.parse_args()

if options.instance:
        instance = options.instance

if options.sectors:
        numsectors = options.sectors
        
if options.stardocks:
        numstardocks = options.stardocks

if options.database:
        database = options.database

if options.connectivity:
        connectivity = options.connectivity

if options.connect_range:
        connect_range = options.connect_range


fnames = ("Joe","Mary","Rosie","Jake","Jim","Bob","Steve","Dan","Jon","Ted","Bubba","Billy","Juzzle","Jacob","Walt","Mike","Jerry","Becky","Henry","Harry","Jack", "John","Muxxl","Juust", "Fruzzn","Gilbert","Yub","Wuk","Pib","Don","Bobo","Vince","Todd","Barry","Brian","Evelyn","Sam","Rob","Wid","Blip","Sorn","Mulg","Wep","Frank","Fred","Jeff","Beepo", "Kooner","Wulp","Vrik", "Balgo","Noos","Nuster", "Jhup","Bazz","Jert","Kevin","Meg","Stew","Buddy", "Wallace","Dirk","Nug","Cul","Wan","Puzz", "Stim", "Mave", "Sherman", "Scoob", "Yuuder", "Remmy", "Shludge", "Gus", "Manny", "Walter", "Wes", "Rusty","Ron","Rich","Rog","Thor","Peter","Paul","Pablo","Josh","Nubs","Michael","Stephanie","Meagan","Mark","Nick","Greg","Chris","Tom","Tuddles","Bowster","Clive","Hank","Sed","William","Buster","Biggy","Tim","Jen","Oscar","Larry","Lou","Lee","Bugnus","Vmeer", "Calvin", "Marty", "George", "Stan", "Ian", "Dave", "Doug", "Scott", "Amy", "Aaron", "Ward", "Bucko", "Sly", "Biff", "Wally", "Dusty", "Gordo", "Rod", "Beef", "Ned", "Bartelby", "Beau", "Suub", "Mip", "Glarg", "Turbo", "Derek", "Eric", "Pleep", "Tugboat", "Ray", "Radar", "Xogan", "Logan", "Xeet", "Connor", "Buzz", "Stoob", "Honk", "Darryl", "Dwight", "Fishface", "Uncle", "Jean-Luc", "Darth", "Noaki", "Hosei", "Kenji", "Ken", "Meagan", "Jack", "Filbar", "Phillip", "Hilmar", "Mario", "Stoonian", "Jiro", "Hitoshi", "Paul", "Gordon", "Nate", "Eric", "Andy", "Gene", "Cat", "Ned", "Abram", "Alexander", "Alexei", "Maria", "Anna", "Veronica", "Boris", "Dmitry", "Igor", "Ivan", "Enzo", "Hugo", "Emma", "Jade", "Rocket", "Arthur", "Peter", "Gary", "Shugdub", "Ryan", "Patrick")

lnames = ("McGee", "Malone","McGregor", "O'Toole", "O'Leery","O'Halligan","O'Hare","Sampson", "Smith", "Busterson", "McFly", "West", "East","North", "South", "Moore", "Martin", "Tuskinini", "McPherson", "Skinner", "Stalone", "Walsh", "Halvorson", "Richardson", "Rickman", "Reynolds", "Riley", "Rivera", "Rivers", "Waters", "Jenkins", "Johnson", "James", "Jenson", "Jackson", "Horton", "Hodgekins", "Hutchinson", "Anderson", "Bailey", "Baker", "Brown", "Karl", "Caldwell", "Campbell", "Carlson", "Carpenter", "Castro", "Clark", "Coleman", "Conrad", "Cooper", "Cox", "Davis", "Dixon", "Douglas", "Duncan", "Ellis", "Fernandez", "Field", "Freeman", "Garcia", "Gilbert", "Gonzales", "Graves", "Guzman", "Hall", "Hanson", "Harris", "Hill", "Hunter", "Jones", "Kaplan", "King", "Larkin", "Langdon", "Larson", "Lee", "Lewis", "Locatelli", "Mackenzie","MacMillan", "Mann", "Marshall", "Masterson", "Mayer", "McBride", "Merril", "Miller", "Mitchell", "Morris", "Murphy", "Murray", "Nash", "Nichols", "Parker", "Patton", "Peterson", "Quinn", "Ramirez", "Reed", "Reyes", "Ross", "Samuels", "Schmidt", "Silva", "Stanton", "Stone", "Sullivan", "Taylor", "Thompson", "Walters", "Vaughn", "Ward", "Glugman", "Axton", "Mitchell", "Farley", "Cameron", "Sagan", "Shoemaker", "Layton", "Baird", "Huffman", "Liu", "Maxwell", "Thumpkin", "Thomas", "Crosby", "Byrne", "DaVinci", "DiNapoli", "Lovelace", "Maddock", "Madison", "Lansford", "Lang", "Kibrick", "Cook", "Katzkov", "Jefferson", "Spacebuck", "Starwalker", "Night", "Starr", "Skytrader", "Belzar", "McFish", "Steel", "Goldsmith", "Baker", "Picard", "Rush", "Vader", "Cooper", "Lee", "Matsumoto", "Miyamoto", "Tanaka", "Yamazaki", "Hamada", "Summerer", "Hankleburg", "Dods", "Noosh", "Marks", "Snow", "Knight", "Roddenberry", "Goob", "McGoo", "Muller", "Schroeder", "Wolf", "Klein", "Zimmerman", "Hoffman", "Werner", "Shulz", "Weber", "Walter", "Ivanov",  "Popov", "Petrov", "Stepanov", "Dubois", "Bouchard", "Leroy", "Lacroix", "Anderson", "Lightbody", "Ross", "Gygax", "Stewart", "Frakes", "Wheaton");


prefixes = ("Black","Huge","Amazing","Galactic", "Space", "Magic", "Super","Half-price","Junky","Supreme","Dark","Lightspeed","Majestic","Dismal", "Lucky","Mega","Luminous", "Red","Interstellar","Weird", "Wacky","Far-out", "Futuristic", "Delightful", "Crazy", "Awesome", "Wonderful", "Infinite", "Rocking","Memorable", "Quick", "Deluxe", "Convenient", "Lunar", "Outrageous", "Stellar", "Low-price", "Affordable", "Deep Space", "Zooby", "Groovy", "Incredible", "Fancy", "Dank", "Shabby")

suffixes = ("Hut", "Palace", "Mall", "Space Port", "Dungeon", "Spot", "Place", "Madhouse", "Store", "Outpost", "Destination", "Location", "Trade Post", "Marketplace", "Town", "City", "Shop","Outlet", "Five-and-dime", "Stand", "Warehouse", "Bazaar", "Stop-n-go", "Mart", "Landing", "Port", "Station", "Shanty", "Shack" )

stardock_prefixes = ("Dark", "Mega", "Stellar", "Deep Space", "Galactic", "Deep", "Shining", "Luminous", "Gleaming", "Radiant", "Ethereal", "Bright", "Hidden", "Remote", "Distant", "Alpha", "Omega", "Epsilon", "Gamma", "Delta", "Echo")
stardock_suffixes = ("Zero", "Metropolis", "Palace", "Base", "World", "Beacon", "Zone", "Stardock", "Harbor", "Haven", "Terminal", "Station", "Sanctuary", "Central", "Prime")

#print len(lnames) * len(fnames) * len(prefixes) * len(suffixes)

try:
        
        conn=psycopg2.connect("dbname='%s' user='void' password='tiTVPok?'" % (database))
except:
        print "I am unable to connect to the database."


class Sector:
        def __init__(self, point, sector):
                self.point = point
                self.sector = sector
        def getPoint(self):
                return self.point
        def getSector(self):
                return self.sector

def random_syllable():
        syllables = ("a", "e", "i", "o", "u", "sha", "shi", "shu", "she", "sho", "ha", "he",
                     "hu", "he", "ho", "ma", "me", "mu", "me", "mo", "na", "ne", "nu", "ne", "no"
                     "pa", "pi", "pu", "pe", "po", "ba", "be", "bu", "be", "bo", "ta", "chi", "tsu", "te", "to",
                     "ka", "ki", "ku", "ke", "ko", "ga","ge","gu","ge","go", "za", "ze", "zu", "ze", "zo", "da", "di", "do", "de", "do",
                     "ya", "ye", "yu", "ye", "yo", "ji", "je", "ja", "ju", "jo")
        return random.choice(syllables)

def generate_stardock_name():
        return random.choice(stardock_prefixes) + ' ' + random.choice(stardock_suffixes)

def generate_name():
        name = ""
        syllables = random.randint(2,5)
        for s in range(0, syllables):
                name += random_syllable()
        name = name[0].upper() + name[1:]
        return name
outpost_names = set()
def generate_outpost_name():
        while True:
                name = ""
                if random.random() >= 0.99:
                        name = generate_name()
                        name = name[0].upper() + name[1:]
                if random.random() < 0.99:
                        if random.random() > 0.9:
                                name = random.choice(fnames)
                        else:
                                name = random.choice(fnames) + ' ' + random.choice(lnames)
                        
                else:
                        name = generate_name()
                if name.endswith('s') or name.endswith('z'):
                        name = name + '\''
                else:
                        name = name + '\'s'

                
                prefix = ''
                if random.random() < 0.4:
                        prefix = random.choice(prefixes)
                        name = name + ' ' + prefix
                if random.random() < 0.1:
                        name = name + " and " + random.choice(prefixes)
                name = name + " " + random.choice(suffixes)
                if name not in outpost_names:
                        outpost_names.add(name)
                        return name

        return name

def delete_edges():
        try:
                cur = conn.cursor()
                cur.execute("DELETE FROM Edges;")
        except pysocpg2.DatabaseError, e:
                if conn:
                        conn.rollback()
                print 'Error %s' % e
                sys.exit(1)
                
def delete_sectors():
        try:
                cur = conn.cursor()
                cur.execute("DELETE FROM Sectors;")
        except  psycopg2.DatabaseError, e:
                if conn:
                        conn.rollback()
                print 'Error %s' % e
                sys.exit(1)
                
def delete_outposts():
        try:
                cur = conn.cursor()
                cur.execute("DELETE FROM Outpost;")
        except  psycopg2.DatabaseError, e:
                if conn:
                        conn.rollback()
                print 'Error %s' % e

def delete_territory():
        try:
                cur = conn.cursor()
                cur.execute("DELETE FROM Territory;")
        except  psycopg2.DatabaseError, e:
                if conn:
                        conn.rollback()
                print 'Error %s' % e

def delete_stardocks():
        try:
                cur = conn.cursor()
                cur.execute("UPDATE Sectors SET bstardock = 'f', sstardockname = '';")
        except  psycopg2.DatabaseError, e:
                if conn:
                        conn.rollback()
                print 'Error %s' % e


def generate_sectors(numsectors):
        try:
                cur = conn.cursor()
                for sector in range(0, numsectors):
                        cur.execute("INSERT INTO Sectors (nsector,kterritory,bdiscovered) VALUES (%s, NULL, FALSE);", (sector,))
        except  psycopg2.DatabaseError, e:
                if conn:
                        conn.rollback()
                print 'Error %s' % e
                sys.exit(1)
#
def generate_outposts(prevalence):
        try:
                cur = conn.cursor()
                for sector in range(0, numsectors):
                        if random.random() < prevalence:
                                name = generate_outpost_name()
                                pbuysplasma = random.random() >= 0.5
                                pbuysmetals = random.random() >= 0.5
                                pbuyscarbon = random.random() >= 0.5
                                buysplasma = "true"
                                buysmetals = "true"
                                buyscarbon = "true"
                                fplasma = 1.0
                                fmetals = 1.0
                                fcarbon = 1.0
                                if pbuysplasma:
                                        fplasma = random.uniform(buy_range[0], buy_range[1])
                                else:
                                        buysplasma = "false"
                                        fplasma = random.uniform(sell_range[0], sell_range[1])
                                if pbuysmetals:
                                        fmetals = random.uniform(buy_range[0], buy_range[1])
                                else:
                                        buysmetals = "false"
                                        fmetals = random.uniform(sell_range[0], sell_range[1])
                                if pbuyscarbon:
                                        fcarbon = random.uniform(buy_range[0], buy_range[1])
                                else:
                                        buyscarbon = "false"
                                        fcarbon = random.uniform(sell_range[0], sell_range[1])
                                        print "Creating Outpost in sector %d named %s" % (sector,name)
                                cur.execute("INSERT INTO Outpost (sname,ksector,bbuyplasma,bbuymetals,bbuycarbon,fplasmaprice,fmetalsprice,fcarbonprice) VALUES (%s,%s,%s,%s,%s,%s,%s,%s);" , (name, sector, buysplasma, buysmetals, buyscarbon, fplasma, fmetals, fcarbon))
                #                print sql
                
        except RuntimeError as err:
                print err

def calc_spread(start,factor):
    spread = start
    # So, if we keep geting lucky, we can get a bigger and bigger spread. For a chance of 10%, one out of 100 times we will get a much bigger spread
    while True:
        if random.random() < (1.0 / factor):
            spread = spread * factor
        else:
            return spread

def heuristic(a, b):
    (x1, y1) = a
    (x2, y2) = b
    return abs(x1 - x2) + abs(y1 - y2)



def a_star_search(points, adjacency, start, goal):
    frontier = PriorityQueue()
    frontier.put(start, 0)
    came_from = {}
    cost_so_far = {}
    came_from[start] = None
    cost_so_far[start] = 0

    
    while not frontier.empty():
        current = frontier.get()
        
        if current == goal:
                return True

        
        for next in adjacency[current]:
            new_cost = cost_so_far[current] + 1
            if next not in cost_so_far or new_cost < cost_so_far[next]:
                cost_so_far[next] = new_cost
                priority = new_cost + heuristic(points[goal], points[next])
                frontier.put(next, priority)
                came_from[next] = current
    
    return False
    
                
def connect_sectors(numsectors):
        try:
                cur = conn.cursor()
                random_points = [(0,0)] # start with just sector 0, located at 0,0
                size = emptiness * math.sqrt(numsectors);
                print "Universe has a size of %f" % (size)
                # first, lets scatter a bunch of points about a temporary virtual 2D plane
                print "Placing sectors in the universe..."
                for n in range(1,numsectors):
                        random_points.append((random.randint(-size/2.0,size/2.0), random.randint(-size/2.0, -size/20)))
                # then, lets order those points by their distance to Terra (sector 0 at location 0,0)
                sorted_points = sorted(random_points,key = lambda point: math.hypot(0.0 - point[0], 0.0 - point[1]))
                # then, lets assign those points to sectors by their order (so, the sector closest to sector 0 is sector 1, and the furthest is sector N-1, where N is numsectors
                # and while we're at it, lets create a quadtree 
                quadtree = Quadtree( (0,0), size, 20)
                sector_num = 0
                adjacency = defaultdict(list)
                print "Placing sectors..."
                for point in sorted_points:
                        quadtree.add(Sector(point,sector_num))
                        cur.execute("UPDATE Sectors set fx = %s, fy = %s where nsector = %s;", (point[0], point[1], sector_num))
                        sector_num = sector_num + 1
                # put sector 0 in the center. 
                #cur.execute("UPDATE Sectors set fx = 0, fy = 0 where nsector = 0;")
                # then, we go through the sectors and search near them for other sectors to connect to, and we connect to approximately C of them
                for sector in range(0,numsectors):
                        conn_count = int(random.gauss(connectivity, 2))
                        # Find any neighbors within a circle centered on this sector.
                        circle = Circle(sorted_points[sector], connect_range)
                        print "Looking for neighbors of sector %d at %s within %f:" % (sector, sorted_points[sector], connect_range)
                        neighbors = quadtree.findObjects(circle)
                        random.shuffle(neighbors)
                        for i in range(0, min(conn_count, len(neighbors))):
                                adj = neighbors[i].getSector()
                                if adj != sector and not adj in adjacency[sector] and not sector in adjacency[adj]:
                                        #print "    connecting sector %d at %s to %d at %s" % (sector, sorted_points[sector], adj, sorted_points[adj])
                                        adjacency[sector].append(adj) # build adjacency list
                                        adjacency[adj].append(sector)
                                        cur.execute("INSERT INTO Edges (nsector, nedge, nsector2) VALUES (%s, %s, %s);", (sector,len(adjacency[sector])+1, adj))


                # great. Now we have to make sure we have no sectors that are disconnected.
                # the best way to do this is to go through our sectors and make sure they each can reach sector 0.
                # we keep a list of sectors that ARE 0-connected,
                # if we encounter a sector that can't reach sector 0, we add a new connection to it
                # to choose a sector to connect to it, we look through the 0-connected sectors for the one closest to our disconnected sector, and connect that one.
                connected = [0]
                for sector in range(1, numsectors):
                        if a_star_search(sorted_points, adjacency, sector, 0):
                              connected.append(sector)
                        else:
                                print "Sector %d at %s is not connected. Fixing..." % (sector, sorted_points[sector])
                                best = 0
                                closest = size * 3 # A distance away it couldn't possibly be
                                for candidate in connected:
                                        distance = math.hypot(sorted_points[candidate][0] - sorted_points[sector][0], sorted_points[candidate][1] - sorted_points[sector][1])
                                        if distance < closest:
                                                closest = distance
                                                best = candidate
                                adjacency[sector].append(adj) # build adjacency list
                                print "    connecting %d to %d" % (sector, best)
                                cur.execute("INSERT INTO Edges (nsector, nedge, nsector2) VALUES (%s, %s, %s);", (sector,len(adjacency[sector])+1, best))
                                connected.append(sector) # Now we're connected!
                                                              
        except (RuntimeError, TypeError, NameError):
                conn.rollback()
                raise
                
        # and when choosing our C/2 edges, we pick a normal random number centered around our own index, but with a sizeable std deviation so that we can connect to far away sectors potentially. 

def create_stardocks(numstardocks):
        cur = conn.cursor()
        cur.execute("select nsector, (a.c + b.c) as c from (select nsector, count(0) as c from edges group by nsector) as a, (select nsector2, count(0) as c from edges group by nsector2) as b where nsector = nsector2 order by c desc limit %s;", (numstardocks,))
        for row in cur.fetchall():
                name = generate_stardock_name()
                print "Creating Stardock named %s in sector %d" % (name, row[0])
                cur.execute("UPDATE Sectors SET bstardock = 't', sstardockname = %s WHERE nsector = %s;", (name, row[0]))

def mark_adjacent_territory(sector, territory, cur_depth, max_depth):
        cur = conn.cursor()
        cur.execute("UPDATE Sectors set kterritory = %s where nsector = %s;", (territory, sector))
        cur.execute("select nsector2 from edges where nsector = %s union select nsector from edges where nsector2 = %s;", (sector,sector))
        for row in cur.fetchall():
                adj = row[0]
                if cur_depth < max_depth:
                        mark_adjacent_territory(adj, territory, cur_depth +1, max_depth)
                
def mark_territory():
        cur = conn.cursor()
        cur.execute("INSERT INTO Territory (nkey, sname) VALUES (0, 'Federation');")
        # Find all stardocks, plus sector 0, mark all adjacent as federation. 
        cur.execute("SELECT nsector FROM Sectors WHERE bstardock = 't';")
        for row in cur.fetchall():
                sector = row[0]
                mark_adjacent_territory(sector, 0, 0, 2)
        
        
# have to delete this in a particular order due to constraints
                    # Try to connect                

print "BAM! With a thunderous bang, and a horrible zap, the universe, as it was, is voilently destroyed!"
delete_edges()
delete_outposts()                
delete_sectors()
delete_territory()
print
print "BA-BANG!!!!!! Suddenly, there is a huge explosion, a new universe is forming!"
print
print
print "[creating the empty universe]"
generate_sectors(numsectors)
print "[filling the universe with outposts]"
generate_outposts(outpost_prevalence)
print "[connecting sectors]"
connect_sectors(numsectors)
print "[creating stardocks]"
create_stardocks(numstardocks)
print "[claiming territory]"
mark_territory()

conn.commit()
print "You have just witnessed the birth of a new universe!"
