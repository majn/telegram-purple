/**
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111-1301  USA
 */

#include <glib.h>

#include "notify.h"
#include "plugin.h"
#include "version.h"
#include "account.h"
#include "connection.h"

#define PLUGIN_ID "prpl-telegram"

#define TELEGRAM_APP_API_ID 16944
#define TELEGRAM_APP_API_HASH "457b5a190c750ed0a772bc48bbdf75dc"
#define TELEGRAM_TEST_SERVER "173.240.5.253"
#define TELEGRAM_PRODUCTION_SERVER "173.240.5.1"
#define TELEGRAM_DEFAULT_PORT 443
#define TELEGRAM_PUBLIC_KEY "-----BEGIN RSA PUBLIC KEY-----MIIBCgKCAQEAwVACPi9w23mF3tBkdZz+zwrzKOaaQdr01vAbU4E1pvkfj4sqDsm6lyDONS789sVoD/xCS9Y0hkkC3gtL1tSfTlgCMOOul9lcixlEKzwKENj1Yz/s7daSan9tqw3bfUV/nqgbhGX81v/+7RFAEd+RwFnK7a+XYl9sluzHRyVVaTTveB2GazTwEfzk2DWgkBluml8OREmvfraX3bkHZJTKX4EQSjBbbdJ2ZXIsRrYOXfaA+xayEGB+8hdlLmAjbCVfaigxX0CDqWeR1yFL9kwd9P0NsZRPsmoqVwMbMu7mStFai6aIhc3nSlv8kg9qv1m6XHVQY3PnEw+QQtqSIXklHwIDAQAB-----END RSA PUBLIC KEY-----"

#define TELEGRAM_AUTH_MODE_PHONE "phone"
#define TELEGRAM_AUTH_MODE_SMS "sms"

typedef struct {
	PurpleAccount *account;
	PurpleConnection *gc;
	PurpleSslConnection *gsc;
} telegram_conn;
