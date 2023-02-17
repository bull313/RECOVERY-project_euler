python3 /Users/bladick/Desktop/project_euler/cards/lab/poker_solver/hardcoresolver.py $1 $2 $3

python3 /Users/bladick/Desktop/project_euler/cards/lab/poker_solver/solution.py "${1%.json}_solution.json"

rm "${1%.json}_solution.json"
mv "${1%.json}_solution_clean.json" "${1%.json}_solution.json"

