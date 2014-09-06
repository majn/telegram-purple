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

extern NSString *const AIDebugLoggingEnabledNotification;
extern BOOL AIDebugLoggingEnabled;

#define AILog(fmt, args...) do { if (__builtin_expect(AIDebugLoggingEnabled, 0)) AILog_impl(fmt, ##args); } while(0)
#define AILogWithPrefix(sig, fmt, args...) do { if(__builtin_expect(AIDebugLoggingEnabled, 0)) AILogWithPrefix_impl(sig, fmt, ##args); } while(0)
#define AILogBacktrace() do { if(__builtin_expect(AIDebugLoggingEnabled, 0)) AILogBacktrace_impl(); } while(0)
#define AILogWithSignature(fmt, args...) AILogWithSignature_impl(__PRETTY_FUNCTION__, __LINE__, fmt, ##args);
void AIEnableDebugLogging();
BOOL AIDebugLoggingIsEnabled();
void AILogWithSignature_impl (const char *function, int line, NSString *format, ...) __attribute__((format(__NSString__, 3, 4)));
void AILogWithPrefix_impl (const char *signature, NSString *format, ...) __attribute__((format(__NSString__, 2, 3)));
void AILog_impl (NSString *format, ...) __attribute__((format(__NSString__, 1, 2)));
void AILogBacktrace_impl();
