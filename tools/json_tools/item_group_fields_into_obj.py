#!/usr/bin/env python3

#Takes a list of fields from an object and groups them inside a new inline object 

import argparse
import json
import os

args = argparse.ArgumentParser()
args.add_argument("dir", action="store", help="specify json directory")
args.add_argument("modified_type", action="store", help="provide JSON type field to modify")
args.add_argument("fields_dir", action="store", help="specify directory of fields to modify (line-separated list, no quotes)")
args.add_argument("new_object_name", action="store", help="provide new object field name")
args_dict = vars(args.parse_args())


def group_to_object(jo, fieldlist, new_object_name, inline):
    jo[new_object_name] = dict()
    for field in fieldlist:
        if field in jo:
            field_value = jo[field]
            jo[new_object_name][field] = field_value
            del jo[field]
    if len(jo[new_object_name]) == 0:
        if not inline:
            printid = "unidentified object"
            if "id" in jo:
                printid = jo["id"]
            elif "abstract" in jo:
                printid = jo["abstract"]
            print("converted object " + printid + " had no fields, creating empty object")
        else:
            del jo[new_object_name]
            return None
    return jo

def object_type_check(jo, fieldlist, modified_type, new_object_name):
    if jo["type"] != modified_type:
        return None
    relative_obj = None
    proportional_obj = None
    # must be islot, relative, or proportional; no combinations allowed!
    if "relative" in jo:
        relative_obj = group_to_object(jo["relative"], fieldlist, new_object_name, True)
        if relative_obj is not None:
            jo["relative"] = relative_obj
    if "proportional" in jo:
        proportional_obj = group_to_object(jo["proportional"], fieldlist, new_object_name, True)
        if proportional_obj is not None:
            jo["proportional"] = proportional_obj
    no_rel_prop = relative_obj is not None or proportional_obj is not None
    jo = group_to_object(jo, fieldlist, new_object_name, no_rel_prop)
    return jo

def gen_new(path, fieldlist, modified_type, new_object_name):
    change = False
    # Having troubles with this script halting?
    # Uncomment below to find the file it's halting on
    # print(path)
    with open(path, "r", encoding="utf-8") as json_file:
        try:
            json_data = json.load(json_file)
        except json.JSONDecodeError:
            print(f"Json Decode Error at {path}")
            print("Ensure that the file is a JSON file consisting of"
                  " an array of objects!")
            return None

        for jo in json_data:
            if type(jo) is not dict:
                print(f"Incorrectly formatted JSON data at {path}")
                print("Ensure that the file is a JSON file consisting"
                      " of an array of objects!")
                break

            new_data = object_type_check(jo, fieldlist, modified_type, new_object_name)
            if new_data is not None:
                jo = new_data
                change = True

    return json_data if change else None

if __name__ == "__main__":
    fieldlist = open(args_dict["fields_dir"], "r").readlines()
    clean_fieldlist = []
    for field in fieldlist:
        clean_fieldlist.append(field.replace('\n',''))
    
    for root, directories, filenames in os.walk(args_dict["dir"]):
        for filename in filenames:
            if not filename.endswith(".json"):
                continue

            path = os.path.join(root, filename)
            new = gen_new(path, clean_fieldlist, args_dict["modified_type"], args_dict["new_object_name"])
            if new is not None:
                with open(path, "w", encoding="utf-8") as jf:
                    json.dump(new, jf, ensure_ascii=False)
