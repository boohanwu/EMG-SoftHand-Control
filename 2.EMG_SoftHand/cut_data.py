import csv
import os

file="Shaka_10.csv"
start=800
end=14000

def delete_rows(csv_file, start_row, end_row):
    # Create a temporary file to write the filtered rows
    temp_file = "temp.csv"
    
    # Open the input and output files
    with open(csv_file, "r") as file_in, open(temp_file, "w", newline="") as file_out:
        reader = csv.reader(file_in)
        writer = csv.writer(file_out)
        
        # Copy rows that are not within the specified range to the output file
        for i, row in enumerate(reader, start=1):
            if i < start_row or i > end_row:
                writer.writerow(row)
    
    # Replace the original file with the temporary file
    os.remove(csv_file)
    os.rename(temp_file, csv_file)
    
if __name__ == '__main__':
    delete_rows(file, end+2, 15001)
    delete_rows(file, 2, start+1)