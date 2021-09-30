using Plots, CSV, DataFrames, Query, ColorSchemes, StatsPlots
gr()

begin
    data_location = "E:\\csvs\\"
    available_files = readdir(data_location)
end

begin
    temp_files = available_files |> @filter(occursin("c", split(_, '.')[1])) |> collect
    temp_files = temp_files |> @filter(occursin("on", _) || occursin("off", _)) |> collect
    
    matched_files = Set{String}()

    dF = DataFrame()
    for file_name in temp_files
        dataset_specifier = split(split(file_name, ".")[1], "_")
        if dataset_specifier[3] in matched_files
            continue
        end
        push!(matched_files, dataset_specifier[3])
        matching_file = temp_files |> @filter(!occursin(dataset_specifier[1], _) && occursin(dataset_specifier[3], _)) |> collect
        
        file_one = DataFrame(CSV.File(data_location*file_name))
        file_two = DataFrame(CSV.File(data_location*matching_file[1]))

        led_val_diff = Float64[]

        for (l, r) in zip(file_one.blue, file_two.blue)
            append!(led_val_diff, abs(r - l))
        end

        insertcols!(dF, 1, dataset_specifier[3] => led_val_diff[1:30000])
    end
    
    p = boxplot(
        ["28c" "31c" "34c" "37c" "40c" "43c"],
        [dF."28c" dF."31c" dF."34c" dF."37c" dF."40c" dF."43c"],
        ylims=(0, 255),
        labels=false,
        xlabel="Temperature",
        ylabel="Pixel Intensity (0-255)"
    )

    savefig(p, "temp-v-pixel-int.png")
end

begin
    ph_files = available_files |> @filter(occursin("ph", split(_, '.')[1])) |> collect
    ph_files = ph_files |> @filter(occursin("on", _) || occursin("off", _)) |> collect
    
    matched_files = Set{String}()

    dF = DataFrame()
    for file_name in ph_files
        println(file_name)
        dataset_specifier = split(split(file_name, ".")[1], "_")
        if dataset_specifier[3] in matched_files
            continue
        end
        push!(matched_files, dataset_specifier[3])
        matching_file = ph_files |> @filter(!occursin(dataset_specifier[1], _) && occursin(dataset_specifier[3], _)) |> collect
        
        file_one = DataFrame(CSV.File(data_location*file_name))
        file_two = DataFrame(CSV.File(data_location*matching_file[1]))

        led_val_diff = Float64[]

        for (l, r) in zip(file_one.blue, file_two.blue)
            append!(led_val_diff, abs(r - l))
        end

        insertcols!(dF, 1, dataset_specifier[3] => led_val_diff[1:10000])
    end
    
    p = boxplot(
        ["5ph" "5.5ph" "6ph" "6.5ph" "7ph" "7.5ph" "8ph" "8.5ph" "9ph" "10ph"],
        [dF."5ph" dF."55ph" dF."6ph" dF."65ph" dF."7ph" dF."75ph" dF."8ph" dF."85ph" dF."9ph" dF."10ph"],
        ylims=(0, 255),
        xlabel="pH",
        labels=false,
        ylabel="Pixel Intensity (0-255)"
    )

    savefig(p, "pH-v-pixel-int.png")
end

begin
    sal_files = available_files |> @filter(occursin("pp", split(_, '.')[1])) |> collect
    sal_files = sal_files |> @filter(occursin("on", _) || occursin("off", _)) |> collect
    
    matched_files = Set{String}()

    dF = DataFrame()
    for file_name in sal_files
        println(file_name)
        dataset_specifier = split(split(file_name, ".")[1], "_")
        if dataset_specifier[1] in matched_files
            continue
        end
        push!(matched_files, dataset_specifier[1])
        matching_file = sal_files |> @filter(!occursin(dataset_specifier[2], _) && occursin(dataset_specifier[1], _)) |> collect
        
        file_one = DataFrame(CSV.File(data_location*file_name))
        file_two = DataFrame(CSV.File(data_location*matching_file[1]))

        led_val_diff = Float64[]

        for (l, r) in zip(file_one.blue, file_two.blue)
            append!(led_val_diff, abs(r - l))
        end

        insertcols!(dF, 1, dataset_specifier[1] => led_val_diff[1:10000])
    end
    
    p = boxplot(
        ["500ppm" "1500ppm" "5ppt" "7ppt" "15ppt" "20ppt" "35ppt"],
        [dF."500ppm" dF."1500ppm" dF."5000ppm" dF."7ppt" dF."15ppt" dF."20ppt" dF."35ppt"],
        ylims=(0, 255),
        xlabel="Salinity",
        labels=false,
        ylabel="Pixel Intensity (0-255)"
    )

    savefig(p, "salinity-v-pixel-int.png")
end

begin
    turbid_files = available_files |> @filter(occursin("g", split(_, '.')[1])) |> collect
    turbid_files = turbid_files |> @filter(occursin("on", _) || occursin("off", _)) |> collect
    
    matched_files = Set{String}()

    dF = DataFrame()
    for file_name in turbid_files
        println(file_name)
        dataset_specifier = split(split(file_name, ".")[1], "_")
        if dataset_specifier[1] in matched_files
            continue
        end
        push!(matched_files, dataset_specifier[1])
        matching_file = turbid_files |> @filter(!occursin(dataset_specifier[2], _) && occursin(dataset_specifier[1], _)) |> collect
        
        file_one = DataFrame(CSV.File(data_location*file_name))
        file_two = DataFrame(CSV.File(data_location*matching_file[1]))

        led_val_diff = Float64[]

        for (l, r) in zip(file_one.blue, file_two.blue)
            append!(led_val_diff, abs(r - l))
        end

        insertcols!(dF, 1, dataset_specifier[1] => led_val_diff[1:10000])
    end

    p = boxplot(
        ["0g" "2.5g" "5g" "7.5g" "10g" "12.5g" "15g" "30g"],
        [dF."0g" dF."25g" dF."5g" dF."75g" dF."10g" dF."125g" dF."15g" dF."30g"],
        ylims=(0, 255),
        xlabel="Turbidity",
        labels=false,
        ylabel="Pixel Intensity (0-255)"
    )

    savefig(p, "turbid-v-pixel-int.png")
end