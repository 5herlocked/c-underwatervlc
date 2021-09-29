using Plots, CSV, DataFrames, Query
gr()

begin
    data_location = "E:\\csvs\\"
    available_files = readdir(data_location)
end

begin
    temp_files = available_files |> @filter(occursin("c", split(_, '.')[1])) |> collect

    
end