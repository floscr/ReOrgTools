import React from 'react'
import FullCalendar from '@fullcalendar/react'
import dayGridPlugin from '@fullcalendar/daygrid'

// No more loaders! We'll just use createElements
export default ({ events }) => React.createElement(FullCalendar, {
  plugins: [ dayGridPlugin ],
  initialView: "dayGridMonth",
  events,
});
