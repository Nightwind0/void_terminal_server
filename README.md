# void_terminal_server
A server I wrote in C++ for an ANSI terminal based massively multiplayer space 4X game

The server runs as a daemon, accepting socket connections and spinning of threads to service those connections. 
The data is stored in a PostgreSQL database. I use libpqxx++ for the PostgreSQL transactions. 

When you join the game, you are given a spaceship of your very own, and dropped somewhere in space. Space is made up of thousands of
interconnected sectors, filled with outposts, planets, star bases, and other things to discover. Take control of planets
and trade, or become a space pirate and attack other ships. 
