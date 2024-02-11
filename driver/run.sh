while true; do
  pixel_location=$(xdotool getmouselocation | awk '{print substr($1,3, length($1)) " " substr($2,3, length($2))}')
  x=$(echo $pixel_location | awk '{print $1}')
  y=$(echo $pixel_location | awk '{print $2}')

  echo "X is $x and Y is $y"

  pixel_val=$(./main $x $y)
  echo "sending $pixel_val"
  echo -ne $pixel_val > /dev/ttyUSB0
  sleep 1 
done
