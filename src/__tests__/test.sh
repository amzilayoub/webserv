# NOTE:
# 1- Don't frget to create a cgi folder inside the /tmp folder
# 2- Don't forget, trying to upload something, if you odn't have the upload_path set
#    on your config file, it will throws an internal server error
# 3- In wordpress, add in the wp-config.php the following line
# 	 define('CONCATENATE_SCRIPTS', false);
#    in order to solve the style issue



# for the same ports with different server name/host

curl localhost:8000 --resolve localhost:8000:127.0.0.1
curl webserv:8000 --resolve webserv:8000:127.0.0.1

