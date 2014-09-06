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

#import <Adium/AIWindowController.h>
#import <Adium/AILocalizationButton.h>

@interface ESPresetManagementController : AIWindowController {
	IBOutlet		NSTableView		*tableView_presets;

	IBOutlet		NSTextField		*label_editPresets;
	IBOutlet		AILocalizationButton		*button_duplicate;
	IBOutlet		AILocalizationButton		*button_delete;
	IBOutlet		AILocalizationButton		*button_rename;
	IBOutlet		AILocalizationButton		*button_done;
	
	NSArray			*presets;
	NSString		*nameKey;
	
	id				delegate;
	
	NSDictionary	*tempDragPreset;
}

- (id)initWithPresets:(NSArray *)inPresets namedByKey:(NSString *)inNameKey withDelegate:(id)inDelegate;

- (IBAction)duplicatePreset:(id)sender;
- (IBAction)deletePreset:(id)sender;
- (IBAction)renamePreset:(id)sender;

@end

@interface NSObject (ESPresetManagementControllerDelegate)
- (NSArray *)renamePreset:(NSDictionary *)preset toName:(NSString *)newName inPresets:(NSArray *)presets renamedPreset:(id *)renamedPreset;
- (NSArray *)duplicatePreset:(NSDictionary *)preset inPresets:(NSArray *)presets createdDuplicate:(id *)duplicatePreset;
- (NSArray *)deletePreset:(NSDictionary *)preset inPresets:(NSArray *)presets;
@end

@interface NSObject (ESPresetManagementControllerDelegate_Optional)
- (NSArray *)movePreset:(NSDictionary *)preset
				toIndex:(NSUInteger)index
			  inPresets:(NSArray *)presets
		presetAfterMove:(id *)presetAfterMove;
- (BOOL)allowDeleteOfPreset:(NSDictionary *)preset;
- (BOOL)allowRenameOfPreset:(NSDictionary *)preset;
@end
