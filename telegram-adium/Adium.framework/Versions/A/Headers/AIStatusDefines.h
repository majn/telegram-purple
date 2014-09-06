/* 
 * Adium is the legal property of its developers, whose names are listed in the copyright file included
 * with this source distribution.
 * 
 * This program is free software; you can redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with this program; if not,
 * write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
typedef enum {
	AIAvailableStatusType = 0, /* Must be first in the enum */
	AIAwayStatusType,
	AIInvisibleStatusType,
	AIOfflineStatusType
} AIStatusType;
#define STATUS_TYPES_COUNT 4

//Mutability types
typedef enum {
	AIEditableStatusState = 0, /* A user created state which can be modified -- the default, should be 0 */
	AILockedStatusState, /* A state which is built into Adium and can not be modified */
	AITemporaryEditableStatusState, /* A user created state which is not part of the permanent stored state array */
	AISecondaryLockedStatusState /* A state which is managed by Adium and grouped separately from other states of the same type */
} AIStatusMutabilityType;

typedef enum {
	AIAvailableStatusTypeAS = 'Sonl',
	AIAwayStatusTypeAS = 'Sawy',
	AIInvisibleStatusTypeAS = 'Sinv',
	AIOfflineStatusTypeAS = 'Soff'
} AIStatusTypeApplescript;

#define STATUS_UNIQUE_ID					@"Unique ID"
#define	STATUS_TITLE						@"Title"
#define	STATUS_STATUS_TYPE					@"Status Type"
