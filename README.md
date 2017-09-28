# Islandora Daily Stats [![Build Status](https://travis-ci.org/ulsdevteam/islandora_daily_stats.png?branch=7.x)](https://travis-ci.org/ulsdevteam/islandora_daily_stats)

## Installation
This module requires installing a CRON script that will create daily summary results by running Solr queries against the various models and collections and storing these results in MySQL tables.

Before being able to execute the CRON scripts, the uls-tuque.ini must be edited to store the details for your various connections.

The CRON scripts are included in the /cron folder.  They can be run via cron job from their current location, but for security purposes it is recommended that the entire CRON folder is copied to a different location on your server.

>`1 0 * * * /usr/local/bin/cronic /usr/bin/php /opt/islandora_cron/stats-contentmodel-counts.php`
>`5 0 * * * /usr/local/bin/cronic /usr/bin/php /opt/islandora_cron/stats-created-counts.php`



## Usage
When this module is installed and configured, there will be one new result each day.  In order to view the reports, navigate to `/admin/islandora/daily_stats/allmodels`.


## Maintainers/Sponsors
Current maintainers:

* [Brian Gillingham](https://github.com/bgilling)
* [ULS Development Team](https://github.com/ulsdevteam)


## License

[GPLv3](http://www.gnu.org/licenses/gpl-3.0.txt)
