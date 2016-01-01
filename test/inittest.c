/*
 This file is part of telegram-purple

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111-1301  USA

 Copyright Ben Wiederhake 2016
 */

#include <assert.h>
#include <stdio.h>

#include <purple.h>

// Just try to call the init function. If that's successful, then probably a lot of stuff works.
int main(int argc, char **argv) {
  assert(argc == 2);
  printf ("Running inittest on %s.\n", argv[1]);
  PurplePlugin *tgp = purple_plugin_probe(argv[1]);
  if (!tgp) {
      printf ("Looks like there was a problem! Aborting.\n");
      return 1;
  }
  printf ("Looks good. Here's what I can read:\n");
  PurplePluginInfo *info = tgp->info;
#define BLURB(x) printf (#x " = %s\n", info->x);
  BLURB(id)
  BLURB(name)
  BLURB(version)
  BLURB(summary)
  BLURB(description)
  BLURB(author)
  BLURB(homepage)
  return 0;
}
