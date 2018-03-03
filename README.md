# vsop2013-tools
Some tools for parsing VSOP2013 ephemerides.

Mainly helps in dumping it all into an RDBMS so that it could be used in software
with a more convenient representation than a mammoth text or binary file that needs
a special parser to work with.

### vsop-convert
Converts the original ASCII ephemeride files into a fragging mammoth of an SQL statement
that the user can then dump into SQLite to create a database that would have all the coefficients
and, ideally, would allow requesting those for a specific JD.

### libvsop2013
Library for parsing the VSOP2013 data. Currently supports only ASCII format, but once the ASCII
mess is out of the way, binary record format is easy-peasy.
