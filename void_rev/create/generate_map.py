#!/usr/bin/python
import psycopg2
import optparse

from PIL import Image, ImageDraw


# 
sector_color = (255,255,255,255)
federation_color = (0,0,255,255)
outpost_color = (255,255,0,255)
stardock_color = (0,255,255,255)
planet_color = (0,255,0,255)
edge_color = (128,0,0,255)
database = "void_large"
scale = 2
should_draw_labels = True
parser = optparse.OptionParser()
parser.set_defaults(instance='default',command='list')
parser.add_option('--db', action='store', dest='database')
parser.add_option('--scale', action='store', dest='scale')
parser.add_option('--no-labels', action='store_false', dest='no_labels')
(options, args) = parser.parse_args()

if options.database:
        database = options.database

if options.scale:
        scale = options.scale
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
        cur.execute("select a.fx * %s + %s, a.fy * %s + %s, b.fx * %s + %s,b.fy * %s +%s from edges e, (select nsector, fx, fy from sectors) a, (select nsector, fx,fy from sectors) as b where a.nsector = e.nsector and b.nsector = e.nsector2;", (scale,offset,scale,offset,scale,offset,scale,offset))
        for record in cur:
                surface.line(record, fill=edge_color)

    
    
def draw_sectors(surface, offset):
        print "Drawing sectors..."
        cur = conn.cursor()
        cur.execute("select fx * %s + %s, fy * %s + %s, kterritory from sectors;", (scale,offset,scale,offset))
        for record in cur:
                if record[2] == 0:
                        draw_point(surface, (record[0], record[1]), federation_color, 4)
                else:
                        surface.point((record[0], record[1]), fill=sector_color)

#                draw_point(surface, record, sector_color, 2)

def draw_outposts(surface, offset):
        print "Drawing outposts..."
        cur = conn.cursor()
        cur.execute("select fx * %s + %s, fy * %s + %s from outpost, sectors where outpost.ksector = sectors.nsector;",(scale,offset,scale,offset))
        for record in cur:
                #surface.point(record, fill=outpost_color)
                draw_point(surface, record, outpost_color, 3)

def draw_stardocks(surface, offset):
        print "Drawing stardocks..."
        cur = conn.cursor()
        cur.execute("select fx * %s + %s, fy * %s + %s from sectors where bstardock = true;",(scale, offset, scale, offset))
        for record in cur:
                draw_point(surface, record, stardock_color, 6)

def draw_planets(surface, offset):
        print "Drawing planets..."
        cur = conn.cursor()
        cur.execute("select fsize, fx * %s + %s, fy * %s + %s from planet, sectors where planet.ksector = sectors.nsector;", (scale, offset, scale, offset))
        for record in cur:
                draw_point(surface, (record[1], record[2]), planet_color, max(2, record[0] / 3.0))

dimensions =  universe_dimensions()
im = Image.new("RGBA", (int(scale * abs(dimensions[0][0] - dimensions[1][0])), int(scale * abs(dimensions[0][1] - dimensions[1][1]))), "black")
offset = dimensions[1][0] * scale # TODO Cooler way to calculate this?
surface = ImageDraw.Draw(im)
#surface.line((0, im.size[1], im.size[0], 0), fill=outpost_color)
draw_edges(surface, offset)
draw_sectors(surface, offset)
draw_outposts(surface, offset)
draw_stardocks(surface, offset)
draw_planets(surface, offset)


draw_point(surface, (offset,offset), planet_color, 8)


del surface

# write to stdout
im.save("universe.png", "PNG")
