# To setup the project:
- Create a cgi folder inside the /tmp folder
- In wordpress, add in the '''wp-config.php''' the following line '''define('CONCATENATE_SCRIPTS', false)''' in order to solve the style issue

Each functions should be documented as follow:

/**
 * Prints "Hello!" to the console a given number of times
 * @param count the number of times to print the phrase
 * @returns the number of characters that were printed
 */


 and each file should start with something like

/***********************************************************************
* FILENAME :        fmcompres.cpp
*
* DESCRIPTION :
*       File compression and decompression routines.
*
**/


Separate each section like this:

/************************ CONSTRUCTORS ************************/



- 200_OK
– 404_NOT_FOUND
– 411_LENGTH_REQUIRED
– 500_INTERNAL_SERVER_ERROR
– 501_NOT_IMPLEMENTED
– 503_SERVICE_UNAVAILABLE
– 505_HTTP_VERSION_NOT_SUPPORTED
