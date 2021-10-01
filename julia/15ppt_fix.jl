using CSV, Query, DataFrames, Statistics

begin
    data_location = "C:\\Users\\camv7\\Downloads\\MORSE\\csvs\\"
    available = readdir(data_location)
end

begin
    sal_files = available |> @filter(occursin("pp", _) && occursin("15ppt", _)) |> collect

    ground_truth_on = sal_files |> @filter(occursin("on", _)) |> collect

    blue_low = 176

    blue_high = mean(DataFrame(CSV.File(data_location*ground_truth_on[1])).blue)

    threshold = blue_low + (blue_high - blue_low)/2

    println(threshold)

    for file_name in sal_files
        if occursin("on", file_name) || occursin("off", file_name) || occursin("transmitter", file_name)
            continue
        end

        original_file = DataFrame(CSV.File(data_location*file_name))

        dF = DataFrame(original_file)

        show(dF)

        for row in eachrow(dF)
            if row.blue < threshold
                row.bit = 0
            elseif row.blue > threshold
                row.bit = 1
            end
        end

        CSV.write(file_name, dF)
    end
end