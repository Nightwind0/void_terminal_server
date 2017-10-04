#!/usr/bin/python

import os
import optparse
import psycopg2
import random
import sys
import math
import quadtree

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
connect_range = math.sqrt(numsectors) * emptiness * connectivity 

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


fnames = ("Joe","Mary","Rosie","Jake","Jim","Bob","Steve","Dan","Jon","Ted","Bubba","Billy","Juzzle","Jacob","Walt","Mike","Jerry","Becky","Henry","Harry","Jack", "John","Muxxl","Juust", "Fruzzn","Gilbert","Yub","Wuk","Pib","Don","Bobo","Vince","Todd","Barry","Brian","Evelyn","Sam","Rob","Wid","Blip","Sorn","Mulg","Wep","Frank","Fred","Jeff","Beepo", "Kooner","Wulp","Vrik", "Balgo","Noos","Nuster", "Jhup","Bazz","Jert","Kevin","Meg","Stew","Buddy", "Wallace","Dirk","Nug","Cul","Wan","Puzz", "Stim", "Mave", "Sherman", "Scoob", "Yuuder", "Remmy", "Shludge", "Gus", "Manny", "Walter", "Wes", "Rusty","Ron","Rich","Rog","Thor","Peter","Paul","Pablo","Josh","Nubs","Michael","Stephanie","Meghan","Mark","Nick","Greg","Chris","Tom","Tuddles","Bowster","Clive","Hank","Sed","William","Buster","Biggy","Tim","Jen","Oscar","Larry","Lou","Lee","Bugnus","Vmeer", "Calvin", "Marty", "George", "Stan", "Ian", "Dave", "Doug", "Scott", "Amy", "Aaron", "Ward", "Bucko", "Sly", "Biff", "Wally", "Dusty", "Gordo", "Rod", "Beef", "Ned", "Bartelby", "Beau", "Suub", "Mip", "Glarg", "Turbo", "Derek", "Eric", "Pleep", "Tugboat", "Ray", "Radar", "Xogan", "Logan", "Xeet", "Connor", "Buzz", "Stoob", "Honk", "Darryl", "Dwight", "Fishface", "Uncle", "Jean-Luc", "Darth", "Noaki", "Hosei", "Kenji", "Ken", "Meagan", "Jack", "Filbar", "Phillip", "Hilmar", "Mario", "Stoonian", "Jiro", "Hitoshi", "Paul", "Gordon", "Nate", "Eric", "Andy", "Gene", "Cat", "Ned", "Abram", "Alexander", "Alexei", "Maria", "Anna", "Veronica", "Boris", "Dmitry", "Igor", "Ivan", "Enzo", "Hugo", "Emma", "Jade", "Rocket", "Arthur", "Peter", "Gary", "Shugdub", "Ryan", "Patrick")

lnames = ("McGee", "Malone","McGregor", "O'Toole", "O'Leery","O'Halligan","O'Hare","Sampson", "Smith", "Busterson", "McFly", "West", "East","North", "South", "Moore", "Martin", "Tuskinini", "McPherson", "Skinner", "Stalone", "Walsh", "Halvorson", "Richardson", "Rickman", "Reynolds", "Riley", "Rivera", "Rivers", "Waters", "Jenkins", "Johnson", "James", "Jenson", "Jackson", "Horton", "Hodgekins", "Hutchinson", "Anderson", "Bailey", "Baker", "Brown", "Karl", "Caldwell", "Campbell", "Carlson", "Carpenter", "Castro", "Clark", "Coleman", "Conrad", "Cooper", "Cox", "Davis", "Dixon", "Douglas", "Duncan", "Ellis", "Fernandez", "Field", "Freeman", "Garcia", "Gilbert", "Gonzales", "Graves", "Guzman", "Hall", "Hanson", "Harris", "Hill", "Hunter", "Jones", "Kaplan", "King", "Larkin", "Langdon", "Larson", "Lee", "Lewis", "Locatelli", "Mackenzie","MacMillan", "Mann", "Marshall", "Masterson", "Mayer", "McBride", "Merril", "Miller", "Mitchell", "Morris", "Murphy", "Murray", "Nash", "Nichols", "Parker", "Patton", "Peterson", "Quinn", "Ramirez", "Reed", "Reyes", "Ross", "Samuels", "Schmidt", "Silva", "Stanton", "Stone", "Sullivan", "Taylor", "Thompson", "Walters", "Vaughn", "Ward", "Glugman", "Axton", "Mitchell", "Farley", "Cameron", "Sagan", "Shoemaker", "Layton", "Baird", "Huffman", "Liu", "Maxwell", "Thumpkin", "Thomas", "Crosby", "Byrne", "DaVinci", "DiNapoli", "Lovelace", "Maddock", "Madison", "Lansford", "Lang", "Kibrick", "Cook", "Katzkov", "Jefferson", "Spacebuck", "Starwalker", "Night", "Starr", "Skytrader", "Belzar", "McFish", "Steel", "Goldsmith", "Baker", "Picard", "Rush", "Vader", "Cooper", "Lee", "Matsumoto", "Miyamoto", "Tanaka", "Yamazaki", "Hamada", "Summerer", "Hankleburg", "Dods", "Noosh", "Marks", "Snow", "Knight", "Roddenberry", "Goob", "McGoo", "Muller", "Schroeder", "Wolf", "Klein", "Zimmerman", "Hoffman", "Werner", "Shulz", "Weber", "Walter", "Ivanov",  "Popov", "Petrov", "Stepanov", "Dubois", "Bouchard", "Leroy", "Lacroix", "Anderson", "Lightbody", "Ross", "Gygax", "Stewart", "Frakes", "Wheaton");


prefixes = ("Black","Huge","Amazing","Galactic", "Space", "Magic", "Super","Half-price","Junky","Supreme","Dark","Lightspeed","Majestic","Dismal", "Lucky","Mega","Luminous", "Red","Interstellar","Weird", "Wacky","Far-out", "Futuristic", "Delightful", "Crazy", "Awesome", "Wonderful", "Infinite", "Rocking","Memorable", "Quick", "Deluxe", "Convenient", "Lunar", "Outrageous", "Stellar", "Low-price", "Affordable", "Deep Space", "Zooby", "Groovy", "Incredible", "Fancy", "Dank", "Shabby")

suffixes = ("Hut", "Palace", "Mall", "Space Port", "Dungeon", "Spot", "Place", "Madhouse", "Store", "Outpost", "Destination", "Location", "Trade Post", "Marketplace", "Town", "City", "Shop","Outlet", "Five-and-dime", "Stand", "Warehouse", "Bazaar", "Stop-n-go", "Mart", "Landing", "Port", "Station", "Shanty", "Shack" )

stardock_prefixes = ("Dark", "Mega", "Stellar", "Deep Space", "Galactic", "Deep", "Shining", "Luminous", "Gleaming", "Radiant", "Ethereal", "Bright", "Hidden", "Remote", "Distant", "Alpha", "Omega", "Epsilon", "Gamma", "Delta", "Echo")
stardock_suffixes = ("Zero", "Metropolis", "Palace", "Base", "World", "Beacon", "Zone", "Stardock", "Harbor", "Haven", "Terminal", "Station", "Sanctuary", "Central", "Prime")

#print len(lnames) * len(fnames) * len(prefixes) * len(suffixes)

class Sector:
        def __init__(self, point):
                self.point = point
        def getPoint(self):
                return self.point

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

def generate_outpost_name():
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
        return name
                
# Try to connect                
try:
        
        conn=psycopg2.connect("dbname='%s' user='void' password='tiTVPok?'" % (database))
except:
        print "I am unable to connect to the database."

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
                
def connect_sectors(numsectors):
        try:
                cur = conn.cursor()
                random_points = [(0,0)] # start with just sector 0, located at 0,0
                radius = emptiness * math.sqrt(numsectors);
                # first, lets scatter a bunch of points about a temporary virtual 2D plane
                for n in range(1,numsectors):
                        random_points.append((random.randint(-radius,radius),random.randint(-radius, radius)))
                # then, lets order those points by their distance to Terra (sector 0 at location 0,0)
                sorted_points = sorted(random_points,key = lambda point: math.hypot(0.0 - point[0], 0.0 - point[1]))         
                # then, lets assign those points to sectors by their order (so, the sector closest to sector 0 is sector 1, and the furthest is sector N-1, where N is numsectors
                # and while we're at it, lets create a quadtree 
                quadtree = Quadtree( (0,0), radius, 20)
                sector_num = 0
                for point in sorted_points:
                        quadtree.add(Sector(point))
                        cur.execute("UPDATE Sectors set fx = %s, fy = %s where nsector = %s;", (point[0], point[1], sector_num))
                        sector_num = sector_num + 1
                # put sector 0 in the center. 
                #cur.execute("UPDATE Sectors set fx = 0, fy = 0 where nsector = 0;")
                # then, we go through the sectors and search near them for other sectors to connect to, and we connect to approximately C of them
                for sector in range(0,numsectors):
                        conn_count = random.gauss(connectivity, 2)
                        adj_num = 0
                        # Find any neighbors within a circle centered on this sector.
                        circle = Circle(random_points[sector], connect_range)
                        neighbors = quadtree.findObjects(circle);
                        adjs = set([])
                        for i in range(0, conn_count):
                                adj = random.choice(neighbors)
                                if adj != sector and !adj in adjs:
                                        adjs.add(adj)
                                        cur.execute("INSERT INTO Edge (nsector, nedge, nsector2) VALUES (%s, %s, %s);", (sector,adj_num, adj))
                                        adj_num = adj_num + 1
        except:
                conn.rollback()
                
        # and when choosing our C/2 edges, we pick a normal random number centered around our own index, but with a sizeable std deviation so that we can connect to far away sectors potentially. 

def create_stardocks(numstardocks):
        cur = conn.cursor()
        cur.execute("select sector, (a.c + b.c) as c from (select nsector, count(0) as c from edges group by nsector) as a, (select nsector2, count(0) as c from edges group by nsector2) as b where nsector = nsector2 order by c desc limit %s;", (numstardocks))
        for row in cur.fetchall():
                name = generate_stardock_name()
                cur.execute("UPDATE Sectors SET bstardock = 't', sstardockname = %s WHERE nsector = %s;", (name, row[0]))
def mark_territory():
        cur = conn.cursor()
        cur.execute("INSERT INTO Territory (nkey, sname) VALUES (0, 'Federation');"
        # Find all stardocks, plus sector 0, mark all adjacent as federation. 
        
# have to delete this in a particular order due to constraints
delete_outposts()                
delete_sectors()

generate_sectors(numsectors)
generate_outposts(outpost_prevalence)
connect_sectors(numsectors)
create_stardocks(numstardocks)
mark_territory()

conn.commit()
