The program should be run in x86 debug or release.

DATABASE
=========
Change the Database connection info in the SQLManager.cpp(Authentication Server) file in the constructor, to your own database info.
Import the NetworkingFinal.sql schema into your workbench.
To do this go to your workbench and login, Go to Server on the top bar then to Data Import. Choose the folder that the schema file is in.
Select the schema and hit start Import.

SOLUTION
=========
Make sure in solution properties that the Authserver is set to start first and the game server is set to start second.
Start the application.
Start the client manually by right clicking the project and going to debug->Start new instance.



COMMANDS	(this will all be displayed on the client console)
==========
(Commands(Must be capitalized))		(input separated by space)
REGISTER				email password
AUTHENTICATE				email password
CREATE   				CityName GameMode LobbyName NumPlayers
VIEW				
REFRESH					
JOIN					lobbyname
LEAVE					(if you are in a lobby you will leave it)
QUIT

NOTES:
The user lobby leader name will only show up if they have authenticated before creating the lobby.
There is no checking done for the create options because it wasn't part of the requirements.
To refresh lobbies just use VIEW (doesn't get rid of old info first