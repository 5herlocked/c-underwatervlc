using CSV, DataFrames, Query, Statistics

# load all the available data
begin
    data_location = "E:\\csvs\\"
    available_files = readdir(data_location)
end

off_mean_list = Float64[]
off_var_list = Float64[]
on_mean_list = Float64[]
on_var_list = Float64[]

begin
    for file in available_files
        dF = DataFrame(CSV.File(data_location * file))
        list_off = dF |> @filter(_.bit == 0) |> DataFrame
        list_on = dF |> @filter(_.bit == 1) |> DataFrame

        push!(off_mean_list, mean(list_off.blue))
        push!(off_var_list, var(list_off.blue))
        push!(on_mean_list, mean(list_on.blue))
        push!(on_var_list, var(list_on.blue))
    end
end

show(string(mean(off_mean_list))*"\n")
show(string(var(off_var_list))*"\n")
show(string(mean(on_mean_list))*"\n")
show(string(var(on_var_list))*"\n")
