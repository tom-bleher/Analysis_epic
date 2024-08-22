# path for the default pixel value holding xml file
default_pixel_path = r"/data/tomble/eic/"

# path to hold the copies with the requested pixel values
user_pixel_path = os.path.join(pixelPath, "temp")

# create a backup directory name for previous simulation runs
#SimBackUpPath = f"{simPath}\{datetime.now().strftime("%d%m%Y_%H%M%S")}" 
SimBackUpPath = os.path.join(simPath, datetime.now().strftime("%d%m%Y_%H%M%S"))

# if we have files from a previous run, create a backup and title accordingly 
if any(os.path.isfile(os.path.join(simPath, item)) for item in os.listdir(simPath)):
	os.mkdir(SimBackUpPath)
	for file in os.listdir(simPath): 
		if os.path.isfile(file):
			shutil.move(os.path.join(simPath, file), SimBackUpPath)
  print("Created new back up simulation files in {0}".format(SimBackUpPath))

# check if path exists and create it if not
if not os.path.exists(user_pixel_path):
	os.mkdir(user_pixel_path)
# if it exists, delete all its contents
else:
	for xml in os.listdir(user_pixel_path):
		if os.path.isfile(xml):
			os.remove(os.path.join(user_pixel_path, xml))


# prompt user to give pixel values
user_pixel_input = list(input("Enter the desired pixel values seperated by commas. You may press enter to run with the default value."))

# if the user did not request any specific values then run with default value
default_pixel_value = 0.7

# if the user did not enter anything, we work with the default value
if len(user_pixel_input) == 0:
	pixel_values_list.append(default_pixel_value)

# take user inputs which are currently in str and transform to list with floats
pixel_values_list = [float(value) for value in user_pixel_input if "," not in value]

# sanity check to see if user inputs were correct
for item in pixel_values_list:
	if type(item) not float:
	    print("Something is wrong with your input. Only int/float (or enter) inputs are valid.")

# loop over all values and copy to dedicated directory
for pixel_value in pixel_values_list:
	new_file_name = f"{os.path.basename(default_pixel_path)}_{str(pixel_value)}"
	new_file_path = os.path.join(user_pixel_path, new_file_name) 

	# make copies with names according to the pixel value which we will now write
	shutil.copy(default_pixel_path, new_file_path)

# loop over the copied files and replace the default value with the user input
for xml, pixel_value in zip(os.listdir(user_pixel_path), pixel_values_list):
	with open(xml, "r+") as file:
		content = file.read()
		# replace the default value with the new value
		content = content.replace('shot_number = ""', f'shot_number = "{shot_name}"')
		file.seek(0)
		file.write(content)
		file.truncate()	

# now we need to run the simulation for each pixel value
