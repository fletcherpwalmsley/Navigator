'''
Terribly hacking Labelme to trainable dataset script

Set @input_mask_dir_path to the dir containing saved Labelme JSON files
Set @output_dir_path to were you you want the output dumped
'''
import os
import subprocess
import uuid

# Path to the folder containing JSON files
input_mask_dir_path = "C:\\Users\\fletc\\Pictures\\CNN\\rivers_pictures\\mask"
output_dir_path = "C:\\Users\\fletc\\Pictures\\CNN\\comb_exp_mask"

# List all files in the folder
json_files = [f for f in os.listdir(input_mask_dir_path) if f.endswith(".json")]

# Iterate through each JSON file and convert
for json_file in json_files:
    input_path = os.path.join(input_mask_dir_path, json_file)
    output_path = os.path.join(output_dir_path, str(uuid.uuid1()))
    # os.path.splitext(json_file)[0] + "_output.json"

    # Run labelme_export_json command using subprocess
    subprocess.run(["labelme_export_json", input_path, "-o", output_path])

    print(f"Converted {json_file} to {os.path.basename(output_path)}")
