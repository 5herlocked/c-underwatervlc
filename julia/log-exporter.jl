using CSV, Query, DataFrames

begin
    data_location = "E:\\MORSE datasets\\temp_var\\31c\\"
    files = readdir(data_location) |> @filter(occursin("transmitter", _)) |> collect
end

begin
    for file in files
        opened_file = open(data_location * file)
        delta_time_l = Float64[]
        bit_l = Integer[]
        lines = readlines(opened_file)
        
        split_lines = split.(lines, ":")
        split_lines = split_lines |> @filter(size(_)[1] >= 3 && length(lstrip(rstrip(_[3])))[1] == 1) |> collect
        for line in split_lines
            append!(delta_time_l, parse(Float64, line[1]))
            append!(bit_l, parse(Float64, line[3]))
        end
        file_dF = DataFrame()
        file_dF.deltaTime = delta_time_l
        file_dF.bit = bit_l
        CSV.write(file, file_dF)
    end
end

