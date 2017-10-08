#/usr/bin/python
import psycopg2
import optparse

from PIL import Image, ImageDraw


# 
sector_color = (255,255,255,255)
outpost_color = (255,255,0,255)
stardock_color = (0,255,255,255)
terra_color = (0,255,0,255)
edge_color = (128,0,0,255)
database = "void_large"
parser = optparse.OptionParser()
parser.set_defaults(instance='default',command='list')
parser.add_option('--db', action='store_true', dest='database')

(options, args) = parser.parse_args()

if options.database:
        database = options.database
try:
        
        conn=psycopg2.connect("dbname='%s' user='void' password='tiTVPok?'" % (database))
except:
        print "I am unable to connect to the database."


def universe_dimensions():
    cur = conn.cursor()
    cur.execute("select min(fx), min(fy), max(fx), max(fy) from sectors;");
    rows = cur.fetchall()
    row = rows[0]
    return ((int(row[0]),int(row[1])),(int(row[2]),int(row[3])))



def draw_point(surface, point, color, size):
        box = [(point[0] - size/2, point[1] - size/2),(point[0] + size/2, point[1] + size/2)]
        surface.ellipse(box, color, color)
        
def draw_edges(surface, offset):
        print "Drawing edges..."
        cur = conn.cursor()
        cur.execute("select a.fx + %s, a.fy + %s, b.fx + %s,b.fy +%s from edges e, (select nsector, fx, fy from sectors) a, (select nsector, fx,fy from sectors) as b where a.nsector = e.nsector and b.nsector = e.nsector2;", (offset,offset,offset,offset))
        for record in cur:
                surface.line(record, fill=edge_color)

    
    
def draw_sectors(surface, offset):
        print "Drawing sectors..."
        cur = conn.cursor()
        cur.execute("select fx + %s, fy + %s from sectors;", (offset,offset))
        for record in cur:
                 surface.point(record, fill=sector_color)
#                draw_point(surface, record, sector_color, 2)

def draw_outposts(surface, offset):
        print "Drawing outposts..."
        cur = conn.cursor()
        cur.execute("select fx + %s, fy + %s from outpost, sectors where outpost.ksector = sectors.nsector;",(offset,offset))
        for record in cur:
                #surface.point(record, fill=outpost_color)
                draw_point(surface, record, outpost_color, 3)

def draw_stardocks(surface, offset):
        print "Drawing stardocks..."
        cur = conn.cursor()
        cur.execute("select fx + %s, fy + %s from sectors where bstardock = true;",(offset,offset))
        for record in cur:
                draw_point(surface, record, stardock_color, 6)

dimensions =  universe_dimensions()
im = Image.new("RGBA", (abs(dimensions[0][0] - dimensions[1][0]), abs(dimensions[0][1] - dimensions[1][1])), "black")
offset = dimensions[1][0] # TODO Cooler way to calculate this?
surface = ImageDraw.Draw(im)
#surface.line((0, im.size[1], im.size[0], 0), fill=outpost_color)
draw_edges(surface, offset)
draw_sectors(surface, offset)
draw_outposts(surface, offset)
draw_stardocks(surface, offset)

draw_point(surface, (0,0), terra_color, 8)


del surface

# write to stdout
im.save("universe.png", "PNG")
