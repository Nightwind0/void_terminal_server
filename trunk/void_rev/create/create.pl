#!/usr/bin/perl

#make DB connection
#check if universe has been created (should be a flag in some table)
#if so, warn and delete from all tables if they still want to proceed. (Ask them twice at least)
#If universe is empty, create!!


use strict;
use DBI;



my $numsectors = 5000;

my $dbh = DBI->connect("DBI:Pg:dbname=void", "void", "tiTVPok?") or die "Database connect failed: $!";

my @fnames = ("Joe","Mary","Rosie","Jake","Jim","Bob","Steve","Dan","Jon","Ted","Bubba","Billy","Juzzle","Jacob","Walt","Mike","Jerry","Becky","Henry","Harry","Jack", "John","Muxxl","Juust", "Fruzzn","Gilbert","Yub","Wuk","Pib","Don","Bobo","Vince","Todd","Barry","Brian","Evelyn","Sam","Rob","Wid","Blip","Sorn","Mulg","Wep","Frank","Fred","Jeff","Beepo", "Kooner","Wulp","Vrik", "Balgo","Noos","Nuster", "Jhup","Bazz","Jert","Kevin","Meg","Stew","Buddy", "Wallace","Dirk","Nug","Cul","Wan","Puzz", "Stim", "Mave", "Sherman", "Scoob", "Yuuder", "Remmy", "Shludge", "Gus", "Manny", "Walter", "Wes", "Rusty","Ron","Rich","Rog","Thor","Peter","Paul","Pablo","Josh","Nubs","Michael","Stephanie","Meghan","Mark","Nick","Greg","Chris","Tom","Tuddles","Bowster","Clive","Hank","Sed","William","Buster","Biggy","Tim","Jen","Oscar","Larry","Lou","Lee","Bugnus","Vmeer", "Calvin", "Marty", "George", "Stan", "Ian", "Dave", "Doug", "Scott", "Amy", "Aaron", "Ward", "Bucko", "Sly", "Biff", "Wally", "Dusty", "Gordo", "Rod", "Beef", "Ned", "Bartelby", "Beau", "Suub", "Mip", "Glarg", "Turbo", "Derek", "Eric", "Pleep", "Tugboat", "Ray", "Radar", "Xogan", "Logan", "Xeet", "Connor", "Buzz");

my @lnames = ("McGee", "Malone","McGregor", "O\\'Toole", "O\\'Leery","O\\'Halligan","O\\'Hare","Sampson", "Smith", "Busterson", "McFly", "West", "East","North", "South", "Moore", "Martin", "Tuskinini", "McPherson", "Skinner", "Stalone", "Walsh", "Halvorson", "Richardson", "Rickman", "Reynolds", "Riley", "Rivera", "Rivers", "Waters", "Jenkins", "Johnson", "James", "Jenson", "Jackson", "Horton", "Hodgekins", "Hutchinson", "Anderson", "Bailey", "Baker", "Brown", "Karl", "Caldwell", "Campbell", "Carlson", "Carpenter", "Castro", "Clark", "Coleman", "Conrad", "Cooper", "Cox", "Davis", "Dixon", "Douglas", "Duncan", "Ellis", "Fernandez", "Field", "Freeman", "Garcia", "Gilbert", "Gonzales", "Graves", "Guzman", "Hall", "Hanson", "Harris", "Hill", "Hunter", "Jones", "Kaplan", "King", "Larkin", "Langdon", "Larson", "Lee", "Lewis", "Locatelli", "Mackenzie","MacMillan", "Mann", "Marshall", "Masterson", "Mayer", "McBride", "Merril", "Miller", "Mitchell", "Morris", "Murphy", "Murray", "Nash", "Nichols", "Parker", "Patton", "Peterson", "Quinn", "Ramirez", "Reed", "Reyes", "Ross", "Samuels", "Schmidt", "Silva", "Stanton", "Stone", "Sullivan", "Taylor", "Thompson", "Walters", "Vaughn", "Ward", "Glugman", "Axton", "Mitchell", "Farley", "Cameron", "Sagan", "Shoemaker", "Layton", "Baird", "Huffman", "Liu", "Maxwell", "Thumpkin", "Thomas", "Crosby", "Byrne", "DaVinci", "DiNapoli", "Lovelace", "Maddock", "Madison", "Lansford", "Lang", "Kibrick", "Cook", "Katzkov", "Jefferson", "Spacebuck", "Starwalker", "Night", "Starr", "Skytrader", "Belzar", "McFish", "Steel", "Goldsmith");

my @prefixes = ("Black","Huge","Amazing","Galactic", "Space", "Magic", "Super","Half-price","Junky","Supreme","Dark","Lightspeed","Majestic","Dismal", "Lucky",
		"Mega","Luminous", "Red","Interstellar","Weird", "Wacky","Far-out", "Futuristic", "Delightful", "Crazy", "Awesome", "Wonderful", "Infinite", "Rocking",
		"Memorable", "Quick", "Deluxe", "Convenient", "Lunar", "Outrageous", "Stellar", "Low-price", "Affordable");
my @suffixes = ("Hut", "Palace", "Mall", "Space Port", "Dungeon", "Spot", "Place", "Madhouse", "Store", "Outpost", "Destination", "Location", "Trade Post", "Marketplace", "Town", "City", "Shop","Outlet", "Five-and-dime", "Stand", "Warehouse", "Bazaar", "Stop-n-go", "Mart", "Landing", "Port", "Station", "Shanty" );


sub gaussian_rand {
    my ($u1, $u2); 
    my $w;         
    my ($g1, $g2); 

    do {
        $u1 = 2 * rand() - 1;
        $u2 = 2 * rand() - 1;
        $w = $u1*$u1 + $u2*$u2;
    } while ( $w >= 1 );

    $w = sqrt( (-2 * log($w))  / $w );
    $g2 = $u1 * $w;
    $g1 = $u2 * $w;
    # return both if wanted, else just one
    return wantarray ? ($g1, $g2) : $g1;
}

sub generate_outpost_name
{
    my $prefix;
    my $suffix;


    if(int(rand 5) != 0)
    {
       
	$prefix = $fnames[int rand scalar @fnames];

	my $choice = int rand 5;

	if($choice !=0)
	{
	    $prefix .= ' ' . $lnames[int rand scalar @lnames];
	}
	elsif(int rand 2 == 0)
	{
	    $prefix .= " and " . $fnames[int rand scalar @fnames];
	}

	if($prefix =~ /s$/)
	{
	    $prefix .= "\\' ";
	}
	else
	{
	    $prefix .= "\\'s ";
	}

	$prefix .= $prefixes[int rand scalar @prefixes];
	$suffix = $suffixes[int rand scalar @suffixes];
    }
    else
    {

	$prefix = $prefixes[int rand scalar @prefixes];

	if(int rand 5 != 0)
	{
	    $prefix .= " and " . $prefixes[int rand scalar @prefixes];
	}
	$suffix = $suffixes[int rand scalar @suffixes];
    }

    return $prefix .' ' . $suffix;
}


sub create_database
{
    exec "createdb void";
    exec "createuser void";
}

sub create_outposts
{
    my $num_outposts = shift;

    for(my $o=0;$o<$num_outposts;$o++)
    {
	my $name = generate_outpost_name();


	my $buymetals = int rand 2;
	my $buyplasma = int rand 2;
	my $buycarbon = int rand 2;

	
	my $buyrate = gaussian_rand() * 0.10 + 1;
	my $sellrate = gaussian_rand() * 0.10 + 0.75;

	my $done = 0;


	my $sector;

	while(!$done)
	{
	    $sector = int rand $numsectors;
	    my $sql_check = "SELECT COUNT(*) FROM Outpost WHERE ksector =  $sector OR sname = '$name';";

	    my $query = $dbh->prepare($sql_check) or die "Query check failed $!";

	    $query->execute();
	    

	    my @array = $query->fetchrow_array();

	    $query->finish();

	    if($array[0] == 0) 
	    {
		$done = 1;
	    }
	    

       }

	my $sql = "INSERT INTO Outpost (sname,ksector, bbuyplasma,bbuymetals,bbuycarbon, fbuyrate, fsellrate) VALUES ('$name',$sector,";


	my $type = "(";

	if($buyplasma)
	{
	    $sql .= "TRUE,";
	    
	    $type .= "B";
	}
	else
	{
	    $sql .= "FALSE,";
	    $type .= "S";
	}

	if($buymetals)
	{
	    $sql .= "TRUE,";
	    $type .= "B";
	}
	else
	{
	    $sql .= "FALSE,";
	    $type .= "S";
	}

	if($buycarbon)
	{
	    $sql .= "TRUE,";
	    $type .= "B";
	}
	else
	{
	    $sql .= "FALSE,";
	    $type .= "S";
	}

	$type .= ")";

	$sql .= "$buyrate, $sellrate);";

	my $query = $dbh->prepare($sql) or die "Query check failed $!";

	$query->execute() or die "Outpost insert failed $sql $!";

	$query->finish();


	print "$name in $sector: $type Buy: $buyrate, Sell $sellrate\n";
    }
}


sub create_sectors_db
{
    my $num_sectors = shift;
    for(my $s=0;$s<$num_sectors;$s++)
    {
	my $sql = "INSERT INTO Sectors (NSECTOR,KTERRITORY,BDISCOVERED) VALUES($s,NULL,FALSE);";

	my $sth = $dbh->prepare($sql) or die "Insert failed: $!";
	$sth->execute();
	$sth->finish();
    }
}

sub create_sectors
{
#Act on the "Edges" table
    my $num_sectors = shift;
    my $max_edges = 5;
    my $loopback_rate = 10;#1 in 10


	my $newest_sector = 5;


#create_sectors_db()

    print "SECTOR#\t EDGE\t POINTER\n";
    print "0\t0\t1\n";
    print "0\t1\t2\n";
    print "0\t2\t3\n";
    print "0\t3\t4\n";
    print "0\t4\t5\n";

    my $sql = "";



    
    for(my $v=0;$v<5;$v++)
    {
	my $v2 = $v + 1;

	$sql .= "INSERT INTO Edges (NSECTOR,NEDGE,NSECTOR2) VALUES (0,$v,$v2);";
    }



    my $sth = $dbh->prepare($sql) or die "Insert failed: $!";


    $sth->execute();
    $sth->finish();


    my @queue = (5,4,3,2,1);

    while($newest_sector +1 < $num_sectors)
    {	

	my $cur_sec = pop(@queue);

	my $num_edge = int(rand $max_edges);
	
	if($num_edge + $newest_sector +1 > $num_sectors) 
	{
	    $num_edge -= (($newest_sector +1) - $num_edge);

	}


#	my $sth = $dbh->prepare($sql) or die "Could not create new sector $!";
#	$sth->execute();
#	$sth->finish();

	


	for(my $i=0; $i<$num_edge;$i++)
	{

	    my $sql = "";
	    if(int(rand $loopback_rate) == 0)
	    {
		#my $loopback_point = int(rand $newest_sector);
		my $sdev = 10; # This determines, in general, how far loopback points are from their origin. Usually you will see them connect to neighbors, and not much further
		my $mean = $cur_sec;

		my $loopback_point = int(gaussian_rand() * $sdev + $mean);
		my $sql_check = "SELECT count(0) from edges where (nsector = $cur_sec and  nsector2 = $loopback_point) OR (nsector2 = $cur_sec and nsector = $loopback_point);";
		my $query = $dbh->prepare($sql_check) or die "Query check failed $!";

		$query->execute();
		

		my @array = $query->fetchrow_array();

		$query->finish();

		

		while($loopback_point == $cur_sec || $loopback_point >= $num_sectors || $array[0] > 0) 
		{
		    $loopback_point = int(gaussian_rand() * $sdev + $mean);

		    
		    if($array[0] > 0)
		    {
			$sql_check = "SELECT count(0) from edges where (nsector = $cur_sec and  nsector2 = $loopback_point) OR (nsector2 = $cur_sec and nsector = $loopback_point);";
			$query = $dbh->prepare($sql_check) or die "Query check failed $!";
			
			$query->execute();
			
			
			@array = $query->fetchrow_array();
			
			$query->finish();
		    }
		    
		    
#		    $loopback_point = int(rand $newest_sector);
		}
		print "$cur_sec\t$i\t$loopback_point\n";
		
	        $sql = "INSERT INTO Edges (NSECTOR,NEDGE,NSECTOR2)  VALUES($cur_sec,$i,$loopback_point);";
  	    }
	    else
	    {
		
		unshift(@queue, ++$newest_sector);

		print "$cur_sec\t$i\t$newest_sector\n";


		$sql .= "INSERT INTO Edges (NSECTOR,NEDGE,NSECTOR2) VALUES($cur_sec,$i,$newest_sector);";

	    }


	    my $sthl = $dbh->prepare($sql) or die "Insert failed: $!";


	    $sthl->execute();
	    $sthl->finish();

	}
	



    }

    my $foo = scalar @queue;
    foreach my $dude (@queue)
    {
	

	my $new_edges = int(rand $max_edges);
	for(my $i=0;$i<$new_edges;$i++)
	{
	    my $sdev = 30;
	    my $mean = $dude;
	    my $rand_mode = int(rand 2);#int(rand $max_edges);
	    my $loopback_point;

	    if($rand_mode == 0)
	    {
		$loopback_point = int(gaussian_rand() * $sdev + $mean);
	    }
	    else
	    {
		$loopback_point = int(rand $num_sectors);
	    }

	    my $sql_check = "SELECT count(0) from edges where (nsector = $dude and  nsector2 = $loopback_point) OR (nsector2 = $dude and nsector = $loopback_point);";
	    my $query = $dbh->prepare($sql_check) or die "Query check failed $!";

	    $query->execute();
	    

	    my @array = $query->fetchrow_array();

	    $query->finish();

	    

	    while($loopback_point == $dude || $loopback_point >= $num_sectors || $array[0] > 0) 
	    {
		$loopback_point = int(gaussian_rand() * $sdev + $mean);


		if($array[0] > 0)
		{
		    $sql_check = "SELECT count(0) from edges where (nsector = $dude and  nsector2 = $loopback_point) OR (nsector2 = $dude and nsector = $loopback_point);";
		    $query = $dbh->prepare($sql_check) or die "Query check failed $!";

		    $query->execute();
		    

		    @array = $query->fetchrow_array();

		    $query->finish();
		}



	    }
		print "$dude\t$i\t$loopback_point\n";
		my $sth = $dbh->prepare("INSERT INTO Edges (NSECTOR,NEDGE,NSECTOR2) VALUES ($dude,$i,$loopback_point);");

		$sth->execute();
		$sth->finish();


	}



    }
}

sub claim_territory()
{
    # Will act on the "SectorData" table
    # Sends a "probe" to a random location in space
    # Perform a breadth-first traversal of sectors from there, claiming them as you go, and keeping total
    # Once the total reaches the number of sectors defined for a particular territory, loop to the next territory.
    # "Federation" territory should always probe from 0.
}


srand(time());
&create_database();
&create_sectors_db();
&create_sectors($numsectors) ;
&create_outposts(500);

$dbh->disconnect();
